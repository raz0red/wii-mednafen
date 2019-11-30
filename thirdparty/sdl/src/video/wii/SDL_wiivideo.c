/*
SDL - Simple DirectMedia Layer
Copyright (C) 1997-2006 Sam Lantinga

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Tantric, 2009
*/
#include "SDL_config.h"

// Standard includes.
#include <math.h>

// SDL internal includes.
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"
#include "SDL_timer.h"
#include "SDL_thread.h"

// SDL Wii specifics.
#include <gccore.h>
#include <ogcsys.h>
#include <malloc.h>
#include <ogc/texconv.h>
#include <wiiuse/wpad.h>
#include "SDL_wiivideo.h"
#include "SDL_wiievents_c.h"

static const char	WIIVID_DRIVER_NAME[] = "wii";
static lwp_t videothread = LWP_THREAD_NULL;
static SDL_mutex * videomutex = 0;

/*** SDL ***/
static SDL_Rect mode_320;
static SDL_Rect mode_640;

static SDL_Rect* modes_descending[] =
{
  &mode_640,
  &mode_320,
  NULL
};

static GXRModeObj* grxModes[4] = { NULL, NULL, NULL, NULL };

#define DEFAULT_MODE    0
#define STANDARD_MODE   1
#define DSTRIKE_MODE    2
#define INTERLACE_MODE  3

/*** 2D Video ***/
#define HASPECT 			320
#define VASPECT 			240
#define TEXTUREMEM_SIZE 	(640*480*4)

unsigned int *xfb[2] = { NULL, NULL }; // Double buffered
int whichfb = 0; // Switch
GXRModeObj* vmode = 0;
u8 * screenTex = NULL; // screen capture
static int quit_flip_thread = 0;
static unsigned char texturemem[TEXTUREMEM_SIZE] __attribute__((aligned(32))); // GX texture
static unsigned char textureconvert[TEXTUREMEM_SIZE] __attribute__((aligned(32))); // 565 mem

static BOOL renderScreen = TRUE;
static void (*prerendercallback)(void) = NULL;
static void (*rendercallback)(void) = NULL;
static int rotation = 0;
static BOOL filterDisplay = FALSE;

/*** GX ***/
#define DEFAULT_FIFO_SIZE 256 * 1024
static unsigned char gp_fifo[DEFAULT_FIFO_SIZE] __attribute__((aligned(32)));
static GXTexObj texobj;
Mtx gx_view;

/* New texture based scaler */
typedef struct tagcamera
{
  guVector pos;
  guVector up;
  guVector view;
}
camera;

/*** Square Matrix
This structure controls the size of the image on the screen.
Think of the output as a -80 x 80 by -60 x 60 graph.
***/
static s16 square[] ATTRIBUTE_ALIGN (32) =
{
  /*
  * X,   Y,  Z
  * Values set are for roughly 4:3 aspect
  */
  -HASPECT,  VASPECT, 0,	// 0
  HASPECT,  VASPECT, 0,	// 1
  HASPECT, -VASPECT, 0,	// 2
  -HASPECT, -VASPECT, 0	// 3
};


static camera cam = {
  {0.0F, 0.0F, 0.0F},
  {0.0F, 0.5F, 0.0F},
  {0.0F, 0.0F, -0.5F}
};

#define DEBUG 0

#if DEBUG
char debugMsg[512] = "";
static void (*debugcallback)(char* msg) = NULL;
static void writeDebugMsg( char* msg );
#endif

/****************************************************************************
* Scaler Support Functions
***************************************************************************/
static int currentwidth;
static int currentheight;
static int currentbpp;

static void SetupGX();

static void draw_init()
{
  GX_ClearVtxDesc ();

  GX_SetVtxDesc (GX_VA_POS, GX_INDEX8);
  GX_SetVtxDesc (GX_VA_CLR0, GX_INDEX8);
  GX_SetVtxDesc (GX_VA_TEX0, GX_DIRECT);

  GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
  GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
  GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

  GX_SetArray (GX_VA_POS, square, 3 * sizeof (s16));

  GX_SetNumTexGens (1);
  GX_SetNumChans ( 0 );

  GX_SetTexCoordGen (GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

  GX_SetTevOp (GX_TEVSTAGE0, GX_REPLACE);
  GX_SetTevOrder (GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLORNULL );

  memset (&gx_view, 0, sizeof (Mtx));
  guLookAt(gx_view, &cam.pos, &cam.up, &cam.view);
  GX_LoadPosMtxImm (gx_view, GX_PNMTX0);

  GX_InvVtxCache ();	// update vertex cache

  // initialize the texture obj we are going to use
  if (currentbpp == 8 || currentbpp == 16)
    GX_InitTexObj (&texobj, texturemem, currentwidth, currentheight, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
  else
    GX_InitTexObj (&texobj, texturemem, currentwidth, currentheight, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);

#if DEBUG
  snprintf( debugMsg, sizeof(debugMsg), "Texture: %dx%d\n", currentwidth, currentheight );
  writeDebugMsg( debugMsg );
#endif

  GX_LoadTexObj (&texobj, GX_TEXMAP0);	// load texture object so its ready to use
}

static inline void draw_vert (u8 pos, u8 c, f32 s, f32 t)
{
  GX_Position1x8 (pos);
  GX_Color1x8 (c);
  GX_TexCoord2f32 (s, t);
}

static inline void draw_square (Mtx v)
{
  Mtx m, m1;			// model matrix.
  Mtx mv;			// modelview matrix.

  guMtxIdentity (m);
  guVector axis = (guVector) { 0, 0, 1 };
  guMtxRotAxisDeg( m, &axis, rotation );
  guMtxTransApply (m, m, 0, 0, -200);
  guMtxConcat (v, m, mv);

  GX_LoadPosMtxImm (mv, GX_PNMTX0);
  GX_Begin (GX_QUADS, GX_VTXFMT0, 4);
  draw_vert (0, 0, 0.0, 0.0);
  draw_vert (1, 0, 1.0, 0.0);
  draw_vert (2, 0, 1.0, 1.0);
  draw_vert (3, 0, 0.0, 1.0);
  GX_End ();
}

static void * flip_thread (void *arg)
{
  while(1)
  {
    if(quit_flip_thread)
      break;

    if( prerendercallback )
    {
      (*prerendercallback)();
    }

    // clear texture objects
    GX_InvVtxCache();
    GX_InvalidateTexAll();

    SDL_mutexP(videomutex);

    if( renderScreen )
    {
      GX_SetBlendMode(GX_BM_BLEND,GX_BL_DSTALPHA,GX_BL_INVSRCALPHA,GX_LO_CLEAR);  
      GX_SetVtxDesc (GX_VA_POS, GX_INDEX8);
      GX_SetVtxDesc (GX_VA_CLR0, GX_INDEX8);
      GX_SetVtxDesc (GX_VA_TEX0, GX_DIRECT);
      GX_SetTevOp (GX_TEVSTAGE0, GX_REPLACE);

      GX_SetNumChans (0);
      GX_SetTevOrder (GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLORNULL);

      // load texture into GX
      DCFlushRange(texturemem, TEXTUREMEM_SIZE);

      if( vmode != grxModes[DEFAULT_MODE] || !filterDisplay )
      {
        GX_InitTexObjLOD(&texobj,GX_NEAR,GX_NEAR_MIP_NEAR,0.0,10.0,0.0,GX_FALSE,GX_FALSE,GX_ANISO_1);
      }
      GX_LoadTexObj(&texobj, GX_TEXMAP0);    

      draw_square(gx_view); // render textured quad
    }

    if( rendercallback /*&& vmode == grxModes[DEFAULT_MODE]*/ )
    {
      GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);  
      GX_SetTevOp (GX_TEVSTAGE0, GX_PASSCLR);
      GX_SetVtxDesc (GX_VA_TEX0, GX_NONE);    
      GX_SetNumChans (1);
      GX_SetTevOrder (GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0 );

      (*rendercallback)();
    }

    GX_SetColorUpdate(GX_TRUE);

    whichfb ^= 1;

    GX_CopyDisp(xfb[whichfb], GX_TRUE);
    GX_DrawDone();
    SDL_mutexV(videomutex);

    VIDEO_SetNextFramebuffer(xfb[whichfb]);
    VIDEO_Flush();
    VIDEO_WaitVSync();
  }
  return NULL;
}

static void StartVideoThread()
{
  if(videothread == LWP_THREAD_NULL)
  {
    quit_flip_thread = 0;
    LWP_CreateThread (&videothread, flip_thread, NULL, NULL, 0, 68);
  }
}

static int WII_VideoInit(_THIS, SDL_PixelFormat *vformat)
{
  // Set up the modes.
  mode_640.w = 640; //vmode->fbWidth;
  mode_640.h = 480; //vmode->xfbHeight;
  mode_320.w = mode_640.w / 2;
  mode_320.h = mode_640.h / 2;

  // Set the current format.
  vformat->BitsPerPixel	= 16;
  vformat->BytesPerPixel = 2;

  this->hidden->buffer = NULL;
  this->hidden->width = 0;
  this->hidden->height = 0;
  this->hidden->pitch = 0;

  /* We're done! */
  return(0);
}

static SDL_Rect **WII_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags)
{
  return &modes_descending[0];
}

static SDL_Surface *WII_SetVideoMode(_THIS, SDL_Surface *current,
                                     int width, int height, int bpp, Uint32 flags)
{
  if( height > 480 ) 
  {
    height = 480;
  }

  SDL_Rect* 		mode;
  size_t			bytes_per_pixel;
  Uint32			r_mask = 0;
  Uint32			b_mask = 0;
  Uint32			g_mask = 0;

#if DEBUG
  snprintf( debugMsg, sizeof(debugMsg), "WII_SetVideoMode: %d, %d, %d\n", width, height, bpp );
  writeDebugMsg( debugMsg );
  snprintf( debugMsg, sizeof(debugMsg), "tvMode:%d\n", vmode->viTVMode >> 2 );
  writeDebugMsg( debugMsg );
#endif

  // Find a mode big enough to store the requested resolution
  mode = modes_descending[0];
  while (mode)
  {
    if (mode->w == width && mode->h == height)
      break;
    else
      ++mode;
  }

  // Didn't find a mode?
  if (!mode)
  {
    SDL_SetError("Display mode (%dx%d) is unsupported.",
      width, height);
    return NULL;
  }

#if DEBUG
  snprintf( debugMsg, sizeof(debugMsg), "Using mode: %d, %d\n", mode->w, mode->h );
  writeDebugMsg( debugMsg );
#endif

  if(bpp != 8 && bpp != 16 && bpp != 32)
  {
    SDL_SetError("Resolution (%d bpp) is unsupported (8/16/32 bpp only).",
      bpp);
    return NULL;
  }

  bytes_per_pixel = bpp / 8;

  // Free any existing buffer.
  if (this->hidden->buffer)
  {
    free(this->hidden->buffer);
    this->hidden->buffer = NULL;
  }

  // Allocate the new buffer.
  this->hidden->buffer = memalign(32, width * height * bytes_per_pixel);
  if (!this->hidden->buffer )
  {
    SDL_SetError("Couldn't allocate buffer for requested mode");
    return(NULL);
  }

  // Allocate the new pixel format for the screen
  if (!SDL_ReallocFormat(current, bpp, r_mask, g_mask, b_mask, 0))
  {
    free(this->hidden->buffer);
    this->hidden->buffer = NULL;

    SDL_SetError("Couldn't allocate new pixel format for requested mode");
    return(NULL);
  }

  // Clear the buffer
  SDL_memset(this->hidden->buffer, 0, width * height * bytes_per_pixel);

  // Set up the new mode framebuffer
  current->flags = (flags & SDL_DOUBLEBUF) | (flags & SDL_FULLSCREEN) | (flags & SDL_HWPALETTE);
  current->w = width;
  current->h = height;
  current->pitch = current->w * bytes_per_pixel;
  current->pixels = this->hidden->buffer;

  /* Set the hidden data */
  this->hidden->width = current->w;
  this->hidden->height = current->h;
  this->hidden->pitch = current->pitch;

  currentwidth = current->w;
  currentheight = current->h;
  currentbpp = bpp;
  WPAD_SetVRes(WPAD_CHAN_ALL, currentwidth*2, currentheight*2);
  draw_init();
  StartVideoThread();
  /* We're done */
  return(current);
}

/* We don't actually allow hardware surfaces other than the main one */
static int WII_AllocHWSurface(_THIS, SDL_Surface *surface)
{
  return(-1);
}

static void WII_FreeHWSurface(_THIS, SDL_Surface *surface)
{
  return;
}

static int WII_LockHWSurface(_THIS, SDL_Surface *surface)
{
  return(0);
}

static void WII_UnlockHWSurface(_THIS, SDL_Surface *surface)
{
  return;
}

static inline void Set_RGBAPixel(_THIS, int x, int y, u32 color)
{
  u8 *truc = (u8*) texturemem;
  int width = this->hidden->width;
  u32 offset;

  offset = (((y >> 2) << 4) * width) + ((x >> 2) << 6) + (((y % 4 << 2) + x % 4) << 1);

  *(truc + offset) = color & 0xFF;
  *(truc + offset + 1) = (color >> 24) & 0xFF;
  *(truc + offset + 32) = (color >> 16) & 0xFF;
  *(truc + offset + 33) = (color >> 8) & 0xFF;
}

static inline void Set_RGB565Pixel(_THIS, int x, int y, u16 color)
{
  u8 *truc = (u8*) texturemem;
  int width = this->hidden->width;
  u32 offset;

  offset = (((y >> 2) << 3) * width) + ((x >> 2) << 5) + (((y % 4 << 2) + x % 4) << 1);

  *(truc + offset) = (color >> 8) & 0xFF;
  *(truc + offset + 1) = color & 0xFF;
}

static void UpdateRect_8(_THIS, SDL_Rect *rect)
{
  u8 *src;
  u8 *ptr;
  u16 color;
  int i, j;
  Uint16 *palette = this->hidden->palette;
  for (i = 0; i < rect->h; i++)
  {
    src = (this->hidden->buffer + (this->hidden->width * (i + rect->y)) + (rect->x));
    for (j = 0; j < rect->w; j++)
    {
      ptr = src + j;
      color = palette[*ptr];
      Set_RGB565Pixel(this, rect->x + j, rect->y + i, color);
    }
  }
}

static void UpdateRect_16(_THIS, SDL_Rect *rect)
{
  u8 *src;
  int i, j, x, y, p1, p2;
  int width = this->hidden->width;
  for (i = 0; i < rect->h; i++)
  {
    y = rect->y + i;
    src = (this->hidden->buffer + ((width<<1) * y) + (rect->x<<1));    
    p1 = (((y >> 2) << 3) * width);
    p2 = (y % 4 << 2);
    for (j = 0; j < rect->w; j++)
    {
      x = rect->x + j;
      *((u16*)(((u8*)texturemem) + p1 + 
        ((x >> 2) << 5) + ((p2 + x % 4) << 1))) =
        *((u16*)(src + (j<<1)));

    }
  }
}

static void UpdateRect_32(_THIS, SDL_Rect *rect)
{
  u8 *src;
  u8 *ptr;
  u32 color;
  int i, j;
  for (i = 0; i < rect->h; i++)
  {
    src = (this->hidden->buffer + (this->hidden->width * 4 * (i + rect->y)) + (rect->x * 4));
    for (j = 0; j < rect->w; j++)
    {
      ptr = src + (j * 4);
      color = (ptr[1] << 24) | (ptr[2] << 16) | (ptr[3] << 8) | ptr[0];
      Set_RGBAPixel(this, rect->x + j, rect->y + i, color);
    }
  }
}

static void flipHWSurface_16_16(_THIS, SDL_Surface *surface);

static void WII_UpdateRect(_THIS, SDL_Rect *rect)
{
  const SDL_Surface* const screen = this->screen;
  SDL_mutexP(videomutex);
  switch(screen->format->BytesPerPixel) {
  case 1:
    UpdateRect_8(this, rect);
    break;
  case 2:
    flipHWSurface_16_16(this, screen /*surface*/);
    //UpdateRect_16(this, rect);
    break;
  case 4:
    UpdateRect_32(this, rect);
    break;
  default:
    fprintf(stderr, "Invalid BPP %d\n", screen->format->BytesPerPixel);
    break;
  }
  SDL_mutexV(videomutex);
}

static void WII_UpdateRects(_THIS, int numrects, SDL_Rect *rects)
{
  int i;
  for (i = 0; i < numrects; i++)
  {
    WII_UpdateRect(this, &rects[i]);
  }
}

static void flipHWSurface_8_16(_THIS, SDL_Surface *surface)
{
  int new_pitch = this->hidden->width * 2;
  long long int *dst = (long long int *) texturemem;
  long long int *src1 = (long long int *) textureconvert;
  long long int *src2 = (long long int *) (textureconvert + new_pitch);
  long long int *src3 = (long long int *) (textureconvert + (new_pitch * 2));
  long long int *src4 = (long long int *) (textureconvert + (new_pitch * 3));
  int rowpitch = (new_pitch >> 3) * 3;
  int rowadjust = (new_pitch % 8) * 4;
  Uint16 *palette = this->hidden->palette;
  char *ra = NULL;
  int h, w;

  // crude convert
  Uint16 * ptr_cv = (Uint16 *) textureconvert;
  Uint8 *ptr = (Uint8 *)this->hidden->buffer;

  for (h = 0; h < this->hidden->height; h++)
  {
    for (w = 0; w < this->hidden->width; w++)
    {
      Uint16 v = palette[*ptr];

      *ptr_cv++ = v;
      ptr++;
    }
  }

  // same as 16bit
  for (h = 0; h < this->hidden->height; h += 4)
  {
    for (w = 0; w < (this->hidden->width >> 2); w++)
    {
      *dst++ = *src1++;
      *dst++ = *src2++;
      *dst++ = *src3++;
      *dst++ = *src4++;
    }

    src1 += rowpitch;
    src2 += rowpitch;
    src3 += rowpitch;
    src4 += rowpitch;

    if ( rowadjust )
    {
      ra = (char *)src1;
      src1 = (long long int *)(ra + rowadjust);
      ra = (char *)src2;
      src2 = (long long int *)(ra + rowadjust);
      ra = (char *)src3;
      src3 = (long long int *)(ra + rowadjust);
      ra = (char *)src4;
      src4 = (long long int *)(ra + rowadjust);
    }
  }
}

static void flipHWSurface_16_16(_THIS, SDL_Surface *surface)
{
  int h, w;
  long long int *dst = (long long int *) texturemem;
  long long int *src1 = (long long int *) this->hidden->buffer;
  long long int *src2 = (long long int *) (this->hidden->buffer + this->hidden->pitch);
  long long int *src3 = (long long int *) (this->hidden->buffer + (this->hidden->pitch * 2));
  long long int *src4 = (long long int *) (this->hidden->buffer + (this->hidden->pitch * 3));
  int rowpitch = (this->hidden->pitch >> 3) * 3;
  int rowadjust = (this->hidden->pitch % 8) * 4;
  char *ra = NULL;

  for (h = 0; h < this->hidden->height; h += 4)
  {
    for (w = 0; w < this->hidden->width; w += 4)
    {
      *dst++ = *src1++;
      *dst++ = *src2++;
      *dst++ = *src3++;
      *dst++ = *src4++;
    }

    src1 += rowpitch;
    src2 += rowpitch;
    src3 += rowpitch;
    src4 += rowpitch;

    if ( rowadjust )
    {
      ra = (char *)src1;
      src1 = (long long int *)(ra + rowadjust);
      ra = (char *)src2;
      src2 = (long long int *)(ra + rowadjust);
      ra = (char *)src3;
      src3 = (long long int *)(ra + rowadjust);
      ra = (char *)src4;
      src4 = (long long int *)(ra + rowadjust);
    }
  }
}

static void flipHWSurface_32_16(_THIS, SDL_Surface *surface)
{
  SDL_Rect screen_rect = {0, 0, this->hidden->width, this->hidden->height};
  WII_UpdateRect(this, &screen_rect);
}

static int WII_FlipHWSurface(_THIS, SDL_Surface *surface)
{
  switch(surface->format->BytesPerPixel)
  {
  case 1:
    flipHWSurface_8_16(this, surface);
    break;
  case 2:
    flipHWSurface_16_16(this, surface);
    break;
  case 4:
    flipHWSurface_32_16(this, surface);
    break;
  default:
    return -1;
  }
  return 1;
}

static int WII_SetColors(_THIS, int first_color, int color_count, SDL_Color *colors)
{
  const int last_color = first_color + color_count;
  Uint16* const palette = this->hidden->palette;
  int     component;

  /* Build the RGB565 palette. */
  for (component = first_color; component != last_color; ++component)
  {
    const SDL_Color* const in = &colors[component - first_color];
    const unsigned int r    = (in->r >> 3) & 0x1f;
    const unsigned int g    = (in->g >> 2) & 0x3f;
    const unsigned int b    = (in->b >> 3) & 0x1f;

    palette[component] = (r << 11) | (g << 5) | b;
  }

  return(1);
}

static void WII_VideoQuit(_THIS)
{
  WII_VideoStop();
  GX_AbortFrame();
  GX_Flush();

  VIDEO_SetBlack(TRUE);
  VIDEO_Flush();
}

static void WII_DeleteDevice(SDL_VideoDevice *device)
{
  SDL_free(device->hidden);
  SDL_free(device);
}

static SDL_VideoDevice *WII_CreateDevice(int devindex)
{
  SDL_VideoDevice *device;

  /* Initialize all variables that we clean on shutdown */
  device = (SDL_VideoDevice *)SDL_malloc(sizeof(SDL_VideoDevice));
  if ( device ) {
    SDL_memset(device, 0, (sizeof *device));
    device->hidden = (struct SDL_PrivateVideoData *)
      SDL_malloc((sizeof *device->hidden));
  }
  if ( (device == NULL) || (device->hidden == NULL) ) {
    SDL_OutOfMemory();
    if ( device ) {
      SDL_free(device);
    }
    return(0);
  }
  SDL_memset(device->hidden, 0, (sizeof *device->hidden));

  videomutex = SDL_CreateMutex();

  /* Set the function pointers */
  device->VideoInit = WII_VideoInit;
  device->ListModes = WII_ListModes;
  device->SetVideoMode = WII_SetVideoMode;
  device->CreateYUVOverlay = NULL;
  device->SetColors = WII_SetColors;
  device->UpdateRects = WII_UpdateRects;
  device->VideoQuit = WII_VideoQuit;
  device->AllocHWSurface = WII_AllocHWSurface;
  device->CheckHWBlit = NULL;
  device->FillHWRect = NULL;
  device->SetHWColorKey = NULL;
  device->SetHWAlpha = NULL;
  device->LockHWSurface = WII_LockHWSurface;
  device->UnlockHWSurface = WII_UnlockHWSurface;
  device->FlipHWSurface = WII_FlipHWSurface;
  device->FreeHWSurface = WII_FreeHWSurface;
  device->SetCaption = NULL;
  device->SetIcon = NULL;
  device->IconifyWindow = NULL;
  device->GrabInput = NULL;
  device->GetWMInfo = NULL;
  device->InitOSKeymap = WII_InitOSKeymap;
  device->PumpEvents = WII_PumpEvents;

  device->free = WII_DeleteDevice;

  return device;
}

static int WII_Available(void)
{
  return(1);
}

VideoBootStrap WII_bootstrap = {
  WIIVID_DRIVER_NAME, "Wii video driver",
  WII_Available, WII_CreateDevice
};

static void waitVSync()
{
  VIDEO_WaitVSync();

  if (vmode->viTVMode & VI_NON_INTERLACE)
    VIDEO_WaitVSync();
  else
    while (VIDEO_GetNextField())
      VIDEO_WaitVSync();
}

void WII_InitVideoSystem()
{
  int i,j;

  /* Initialise the video system */
  VIDEO_Init();

  vmode = VIDEO_GetPreferredMode(NULL);
#if 0
  vmode->fbWidth = 320;
  vmode->viWidth = 640; //408;
  vmode->viXOrigin = (VI_MAX_WIDTH_NTSC - vmode->viWidth ) / 2;
  //vmode = &TVPal574IntDfScale;
#endif
  //vmode = &TVEurgb60Hz480Int;

  switch (vmode->viTVMode >> 2)
  {
  case VI_PAL: // 576 lines (PAL 50hz)
    // display should be centered vertically (borders)
    vmode = &TVPal576IntDfScale;
    vmode->xfbHeight = vmode->viHeight = 480;
    vmode->viYOrigin = (VI_MAX_HEIGHT_PAL - vmode->viHeight)/2;
    break;

  case VI_NTSC: // 480 lines (NTSC 60hz)
    break;

  default: // 480 lines (PAL 60Hz)
    break;
  }

  // Menu video mode
  grxModes[DEFAULT_MODE] = vmode;   
  
  // Standard video mode
  int mode = STANDARD_MODE;
  grxModes[mode] = (GXRModeObj*)malloc( sizeof(GXRModeObj) );
  memcpy( grxModes[mode], grxModes[DEFAULT_MODE], sizeof(GXRModeObj) );

  // Double strike mode
  mode = DSTRIKE_MODE;
  grxModes[mode] = (GXRModeObj*)malloc( sizeof(GXRModeObj) );
  memcpy( grxModes[mode], grxModes[DEFAULT_MODE], sizeof(GXRModeObj) );
  grxModes[mode]->xfbHeight = grxModes[mode]->efbHeight = 240;
  grxModes[mode]->field_rendering = GX_FALSE;
  grxModes[mode]->xfbMode = VI_XFBMODE_SF;
  grxModes[mode]->viTVMode = VI_TVMODE( vmode->viTVMode >> 2, VI_NON_INTERLACE );  
  grxModes[mode]->vfilter[0] = 0;
  grxModes[mode]->vfilter[1] = 0;
  grxModes[mode]->vfilter[2] = 21;
  grxModes[mode]->vfilter[3] = 22;
  grxModes[mode]->vfilter[4] = 21;
  grxModes[mode]->vfilter[5] = 0;
  grxModes[mode]->vfilter[6] = 0;

  switch (vmode->viTVMode >> 2)
  {
  case VI_PAL: // 576 lines (PAL 50hz)
    grxModes[mode]->viHeight = 572;
    grxModes[mode]->xfbHeight = grxModes[mode]->efbHeight = 286;
    grxModes[mode]->viYOrigin = 
      (VI_MAX_HEIGHT_PAL/2 - grxModes[mode]->viHeight/2 ) / 2;
    grxModes[mode]->viTVMode = VI_TVMODE_PAL_DS;
    break;

  case VI_NTSC: // 480 lines (NTSC 60hz)
    grxModes[mode]->viYOrigin = 
      (VI_MAX_HEIGHT_NTSC/2 - grxModes[mode]->viHeight/2 ) / 2;
    break;

  default: // 480 lines (PAL 60Hz)
    grxModes[mode]->viYOrigin = 
      (VI_MAX_HEIGHT_EURGB60/2 - grxModes[mode]->viHeight/2 ) / 2;
    break;
  }

  // Interlace mode
  mode = INTERLACE_MODE;
  grxModes[mode] = (GXRModeObj*)malloc(sizeof(GXRModeObj));
  memcpy( grxModes[mode], grxModes[DEFAULT_MODE], sizeof(GXRModeObj) );
  grxModes[mode]->xfbMode = VI_XFBMODE_DF;
  grxModes[mode]->viTVMode = VI_TVMODE( vmode->viTVMode >> 2, VI_INTERLACE );  
  grxModes[mode]->vfilter[0] = 8;
  grxModes[mode]->vfilter[1] = 8;
  grxModes[mode]->vfilter[2] = 10;
  grxModes[mode]->vfilter[3] = 12;
  grxModes[mode]->vfilter[4] = 10;
  grxModes[mode]->vfilter[5] = 8;
  grxModes[mode]->vfilter[6] = 8;  
#if 0
  for( i = 0; i < 12; i++ )
    for( j = 0; j < 2; j++ )
      grxModes[mode]->sample_pattern[i][j] = 6;
#endif

  vmode = grxModes[DEFAULT_MODE];  

  /* Set up the video system with the chosen mode */
  VIDEO_Configure(vmode);

  xfb[0] = (u32 *) MEM_K0_TO_K1 (SYS_AllocateFramebuffer (vmode));
  xfb[1] = (u32 *) MEM_K0_TO_K1 (SYS_AllocateFramebuffer (vmode));

  VIDEO_ClearFrameBuffer(vmode, xfb[0], COLOR_BLACK);
  VIDEO_ClearFrameBuffer(vmode, xfb[1], COLOR_BLACK);
  VIDEO_SetNextFramebuffer (xfb[0]);

  // Show the screen.
  VIDEO_SetBlack(FALSE);
  VIDEO_Flush();

  waitVSync();

  /*** Clear out FIFO area ***/
  memset (&gp_fifo, 0, DEFAULT_FIFO_SIZE);

  /*** Initialise GX ***/
  GX_Init (&gp_fifo, DEFAULT_FIFO_SIZE);

  SetupGX();
} 

static void SetupGX()
{   
  Mtx44 p;

  GXColor background = { 0, 0, 0, 0xff };
  GX_SetCopyClear (background, 0x00ffffff);
  GX_SetViewport(0, 0, vmode->fbWidth, vmode->efbHeight, 0, 1);
  GX_SetScissor(0, 0, vmode->fbWidth, vmode->efbHeight);
  
  f32 yScale = GX_GetYScaleFactor( vmode->efbHeight, vmode->xfbHeight );
  u16 xfbHeight = GX_SetDispCopyYScale( yScale );
  u16 xfbWidth = vmode->fbWidth;
  if( xfbWidth & 15 )
    xfbWidth = ( xfbWidth & ~15) + 16;

  GX_SetDispCopySrc(0, 0, vmode->fbWidth, vmode->efbHeight);
  GX_SetDispCopyDst(xfbWidth, xfbHeight);
  GX_SetCopyFilter(vmode->aa, vmode->sample_pattern, 
    ( vmode->xfbMode == VI_XFBMODE_SF ? GX_FALSE : GX_TRUE )
    /*( vmode == grxModes[1] ? GX_FALSE : GX_TRUE )*/, vmode->vfilter);

  GX_SetFieldMode(vmode->field_rendering, ((vmode->viHeight == 2 * vmode->xfbHeight) ? GX_ENABLE : GX_DISABLE));

#if DEBUG
    snprintf( debugMsg, sizeof(debugMsg), "((vmode->viHeight == 2 * vmode->xfbHeight):%d\n", (vmode->viHeight == 2 * vmode->xfbHeight) );
    writeDebugMsg( debugMsg );
#endif

  GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
  GX_SetDispCopyGamma(GX_GM_1_0);
  GX_SetCullMode(GX_CULL_NONE);
  GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);  
  GX_SetAlphaUpdate(GX_TRUE);

  GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
  GX_SetColorUpdate(GX_TRUE);
  GX_SetNumChans(1);

  //guOrtho(p, 480/2, -(480/2), -(640/2), 640/2, 100, 1000); // matrix, t, b, l, r, n, f
  guOrtho(p, vmode->efbHeight/2, -(vmode->efbHeight/2), -(vmode->fbWidth/2), vmode->fbWidth/2, 100, 1000 );
#if DEBUG
    snprintf( debugMsg, sizeof(debugMsg), "vmode->efbHeight/2, -(vmode->efbHeight/2), -(vmode->fbWidth/2), vmode->fbWidth/2:%d,%d,%d,%d\n", vmode->efbHeight/2, -(vmode->efbHeight/2), -(vmode->fbWidth/2), vmode->fbWidth/2 );
    writeDebugMsg( debugMsg );
#endif
  GX_LoadProjectionMtx (p, GX_ORTHOGRAPHIC);
}

void WII_VideoStart()
{
  SetupGX();
  draw_init();
  StartVideoThread();
  WPAD_SetVRes(WPAD_CHAN_ALL, currentwidth*2, currentheight*2);
}

void WII_VideoStop()
{
  quit_flip_thread = 1;
  if(videothread == LWP_THREAD_NULL) return;
  LWP_JoinThread(videothread, NULL);
  videothread = LWP_THREAD_NULL;
}

void WII_ChangeSquare(int xscale, int yscale, int xshift, int yshift)
{  
  if( currentwidth == 320 && currentheight == 240 )
  {
    xscale >>= 1;
    yscale >>= 1;
  }

  square[6] = square[3]  =  xscale + xshift;
  square[0] = square[9]  = -xscale + xshift;
  square[4] = square[1]  =  yscale - yshift;
  square[7] = square[10] = -yscale - yshift;
  DCFlushRange (square, 32); // update memory BEFORE the GPU accesses it!
}

static void resetVideoMode( bool waitVsync )
{
    /* Set up the video system with the chosen mode */
    VIDEO_Configure(vmode);

    VIDEO_ClearFrameBuffer(vmode, xfb[whichfb], COLOR_BLACK);
    //VIDEO_ClearFrameBuffer(vmode, xfb[1], COLOR_BLACK);

    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();

    if( waitVsync )
    {
      waitVSync();
    }

    SetupGX();
    draw_init();
}

void updateWidth( int xscale, int yscale, int width )
{
  bool widescreen = grxModes[DEFAULT_MODE]->viWidth != 640;
  bool rotated = ( rotation != 0 && rotation != 180 );  
  int origXscale = xscale;
  int origYScale = yscale;

#if DEBUG
snprintf( 
  debugMsg, sizeof(debugMsg), "widescreen: %d, xscale:%d, width:%d\n", 
  widescreen, xscale, width );
writeDebugMsg( debugMsg );
#endif  

  vmode->viWidth = 
      rotated ? 
        ( ( yscale * width ) / 240 ) : 
        ( ( xscale * width ) / 320 );

  if( widescreen )
  {
    vmode->viWidth = ( vmode->viWidth * 678 ) / 640;
  }

  if( vmode->viWidth > 720 || vmode->viWidth < width )
  {
#if DEBUG
snprintf( debugMsg, sizeof(debugMsg), 
  "Forcing GX scaler:%d,%d,%d\n", xscale, vmode->viWidth, width );
writeDebugMsg( debugMsg );
#endif
    vmode->viWidth = widescreen ? 678 : 640;
    vmode->fbWidth = 640;
  }
  else
  {
    bool loop = true;
    bool doublewidth = false;
    while( loop )
    {
      loop = false;
      if( width <= 320 && ( ( width << 1 ) <= vmode->viWidth ) )
      {
        doublewidth = true;
        width <<= 1;   
        if( rotated )
        {
          yscale = 480;
        }
        else
        {
          xscale = 640; 
        }
      }
      else
      {
        if( rotated ) 
        {
          yscale = 240;
        }
        else
        {
          xscale = 320;
        }
      }

      vmode->fbWidth = ( width & 15 ? ( ( width & ~15) + 16 ) : width );
      if( ( vmode->fbWidth <= 320 ) && ( vmode->viWidth & 15 ) )
      {
        vmode->viWidth = ( vmode->viWidth & ~15) + 16;
        if( !doublewidth )
        {
          loop = true;
        }
      }
    }

    if( vmode->fbWidth != width )
    {
#if DEBUG
int oldViWidth = vmode->viWidth;
#endif

      vmode->viWidth = ( vmode->fbWidth * vmode->viWidth ) / width;

#if DEBUG
snprintf( 
  debugMsg, sizeof(debugMsg), "adjusted viWidth: %d, %d\n", 
  oldViWidth, vmode->viWidth );
writeDebugMsg( debugMsg );
#endif
    } 

    if( vmode->viWidth & 1 )
    {
      vmode->viWidth += 1;
    }

    if( vmode->viWidth > 720 || ( vmode->viWidth < vmode->fbWidth ) )
    {
#if DEBUG
snprintf( debugMsg, sizeof(debugMsg), 
  "Forcing GX scaler:%d,%d,%d\n", xscale, vmode->viWidth, width );
writeDebugMsg( debugMsg );
#endif
      vmode->viWidth = widescreen ? 678 : 640;
      vmode->fbWidth = 640;
      xscale = origXscale;
      yscale = origYScale;
    }

#if DEBUG
snprintf( debugMsg, sizeof(debugMsg), "viWidth: %d, fbWidth:%d\n", 
  vmode->viWidth, vmode->fbWidth );
writeDebugMsg( debugMsg );
#endif
  }

  switch (vmode->viTVMode >> 2)
  {
    case VI_PAL:      
      vmode->viXOrigin = (VI_MAX_WIDTH_PAL - vmode->viWidth ) / 2;
      break;
    case VI_NTSC:
      vmode->viXOrigin = (VI_MAX_WIDTH_NTSC - vmode->viWidth ) / 2;
      break;
    default:
      vmode->viXOrigin = (VI_MAX_WIDTH_EURGB60 - vmode->viWidth ) / 2;
      break;
  }

  WII_ChangeSquare( xscale, yscale, 0, 0 );
  resetVideoMode( false );
}

void WII_SetStandardVideoMode( int xscale, int yscale, int width )
{
  vmode = grxModes[STANDARD_MODE];
  updateWidth( xscale, yscale, width );
}

void WII_SetDoubleStrikeVideoMode( int xscale, int yscale, int width )
{
  vmode = grxModes[DSTRIKE_MODE];
  updateWidth( xscale, yscale, width );
}

void WII_SetInterlaceVideoMode( int xscale, int yscale, int width )
{
  vmode = grxModes[INTERLACE_MODE];
  updateWidth( xscale, yscale, width );
}

void WII_SetDefaultVideoMode()
{
  if( vmode != grxModes[DEFAULT_MODE] )
  {
    vmode = grxModes[DEFAULT_MODE];
    resetVideoMode( true );
  }
}

void WII_SetPreRenderCallback( void (*cb)(void) )
{
  prerendercallback = cb;
}

void WII_SetRenderCallback( void (*cb)(void) )
{
  rendercallback = cb;
}

void WII_SetRenderScreen( BOOL render )
{
  renderScreen = render;
}

void WII_SetRotation( int rot )
{
  rotation = rot;
}

void WII_SetFilter( BOOL b )
{
  filterDisplay = b;
}

void WII_SetWidescreen(int wide)
{
  int width = wide ? 678 : 640;
  int mode;
  for( mode = 0; mode <= STANDARD_MODE; mode++ )
  {
    grxModes[mode]->viWidth = width;
    switch (vmode->viTVMode >> 2)
    {
      case VI_PAL:      
        grxModes[mode]->viXOrigin = (VI_MAX_WIDTH_PAL - width ) / 2;
        break;
      case VI_NTSC:
        grxModes[mode]->viXOrigin = (VI_MAX_WIDTH_NTSC - width ) / 2;
        break;
      default:
        grxModes[mode]->viXOrigin = (VI_MAX_WIDTH_EURGB60 - width ) / 2;
        break;
    }
  }

	VIDEO_Configure( vmode );
	VIDEO_Flush();	

  waitVSync();
}

#if DEBUG
void WII_SetDebugCallback( void (*cb)(char* msg) )
{
  debugcallback = cb;
}

static void writeDebugMsg( char* msg )
{
  if( debugcallback )
  {
    (*debugcallback)( msg );
  }
}
#endif