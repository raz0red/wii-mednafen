/*
WiiMednafen : Wii port of the Mednafen emulator

Copyright (C) 2011 raz0red
 
This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any
damages arising from the use of this software.
 
Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1.	The origin of this software must not be misrepresented; you
must not claim that you wrote the original software. If you use
this software in a product, an acknowledgment in the product
documentation would be appreciated but is not required.

2.	Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3.	This notice may not be removed or altered from any source
distribution.
*/

#include "main.h"
#include "sound.h"
#include "input.h"
#include <trio/trio.h>

#include "wii_app.h"
#include "wii_gx.h"
#include "wii_main.h"
#include "wii_sdl.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"
#include "wii_mednafen_sdl.h"
#include "fileop.h"
#include "vi_encoder.h"

#include "wiimotenotsupported_png.h"

#include "gettext.h"

#include "Emulators.h"

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"  
#endif

extern "C" 
{
void WII_VideoStart();
void WII_VideoStop();
void WII_SetRotation( int rot );
void WII_SetFilter( BOOL filter );
void WII_ChangeSquare(int xscale, int yscale, int xshift, int yshift);
void WII_SetPreRenderCallback( void (*cb)(void) );
void WII_SetDefaultVideoMode();
void WII_SetStandardVideoMode( int xscale, int yscale, int width );
void WII_SetDoubleStrikeVideoMode( int xscale, int yscale, int width );
void WII_SetInterlaceVideoMode( int xscale, int yscale, int width );
void WII_SetWidescreen(int wide);
extern Mtx gx_view;
}

// Forward references
static void gxrender_callback();

// Mednafen external references
extern volatile Uint32 MainThreadID;
extern MDFNSetting DriverSettings[]; 
extern int DriverSettingsSize;
extern std::vector <MDFNSetting> NeoDriverSettings;
extern char *DrBaseDirectory;
extern volatile MDFN_Surface *VTReady;
extern volatile MDFN_Rect *VTLWReady;
extern volatile MDFN_Rect *VTDRReady;
extern MDFN_Rect VTDisplayRects[2];
extern volatile MDFN_Surface *VTBuffer[2];
extern MDFN_Rect *VTLineWidths[2];
extern SDL_Surface *screen;
extern volatile int NeedVideoChange;
extern MDFNGI *MDFNGameInfo;

extern int LoadGame(const char *force_module, const char *path);
extern int GameLoop(void *arg);
extern char *GetBaseDirectory(void);
extern void KillVideo(void);
extern void FPS_Init(void);
extern void MakeMednafenArgsStruct(void);
extern bool CreateDirs(void);
extern void MakeVideoSettings(std::vector <MDFNSetting> &settings);
extern void MakeInputSettings(std::vector <MDFNSetting> &settings);
extern void DeleteInternalArgs(void);
extern void KillInputSettings(void);
extern void CalcFramerates(char *virtfps, char *drawnfps, char *blitfps, size_t maxlen);
extern void FPS_IncBlitted(void);

extern void KillPortInfo();
extern void BuildPortInfo(MDFNGI *gi);

// The wiimote not supported image data
static gx_imagedata* mote_not_supported_idata = NULL;

// A message to display during emulation
static char message[512] = "";

// The message display time
static u32 message_time;

// The last width/height displayed
static Rect lastRect = { 0, 0 };

// Reset controls
static bool resetControls = false;

/*
 * Whether the controls should be reset prior to entering the emulator loop.
 */
void wii_mednafen_set_reset_controls()
{
  resetControls = true;
}

/*
 * Returns the last display rect
 *
 * return   The last display rect
 */
Rect* wii_mednafen_get_last_rect()
{
  return &lastRect;
}

/**
 * Resets the last cached display rect size
 */
void wii_mednafen_reset_last_rect()
{
  memset( &lastRect, 0x0, sizeof( Rect ) );
}

/*
 * Displays a message during emulation
 *
 * message  The message to display
 */
void wii_mednafen_set_message( const char* msg )
{
  if( msg == NULL )
  {
    message_time = 0;        
  }
  else
  {
    message_time = SDL_GetTicks() + 5 * 1000; // 5 secs
    snprintf( message, sizeof(message), "%s", gettextmsg( msg ) );
  }
}

static void free_video()
{
  for( int i = 0; i < 2; i++ )
  {
    if(VTBuffer[i])
    {
      delete VTBuffer[i];
      VTBuffer[i] = NULL;
    }

    if(VTLineWidths[i])
    {
      free(VTLineWidths[i]);
      VTLineWidths[i] = NULL;
    }
  } 
}

/*
 * Initializes the emulator
 */
void wii_mednafen_init()
{
  std::vector<MDFNGI *> ExternalSystems;
  MainThreadID = SDL_ThreadID();

  DrBaseDirectory=GetBaseDirectory();

  MDFNI_printf(_("Starting Mednafen %s\n"), MEDNAFEN_VERSION);
  MDFN_indent(1);
  MDFN_printf(_("Base directory: %s\n"), DrBaseDirectory);
 
  // Look for external emulation modules here.
  if(!MDFNI_InitializeModules(ExternalSystems))
  {
    MDFN_PrintError( "Unable to initialize external modules" );
    exit( 0 );
  }

  for(unsigned int x = 0; x < DriverSettingsSize / sizeof(MDFNSetting); x++)
    NeoDriverSettings.push_back(DriverSettings[x]);

  MakeVideoSettings(NeoDriverSettings);
  MakeInputSettings(NeoDriverSettings);

  if(!(MDFNI_Initialize(DrBaseDirectory, NeoDriverSettings))) 
  {
    MDFN_PrintError( "Error during initialization" );
    exit( 0 );
  }

  MakeMednafenArgsStruct();
  CreateDirs();

  FPS_Init();
}

/*
 * Free resources (closes) the emulator
 */
void wii_mednafen_free()
{
  CloseGame();

  free_video();

  MDFNI_Kill();
  DeleteInternalArgs();
  KillInputSettings();
}

/*
 * Loads the specified game
 *
 * game     The name of the game
 * return   1 if the load is successful, 0 if it fails
 */
int wii_mednafen_load_game( char* game )
{
  return LoadGame( NULL, game );
}

static void reset_video()
{
  free_video();

  Emulator* emu = emuRegistry.getCurrentEmulator();
  Rect* size = emu->getEmulatorScreenSize();
  u8 bpp = emu->getBpp();
  
  back_surface = 
    SDL_SetVideoMode( size->w, size->h, bpp,
      SDL_HWSURFACE | SDL_HWPALETTE | SDL_FULLSCREEN );

  MDFN_PixelFormat nf;
  nf.bpp = bpp;
  nf.colorspace = MDFN_COLORSPACE_RGB;

  if( nf.bpp == 32 ) 
  {
    u8 rs = back_surface->format->Rshift;
    u8 gs = back_surface->format->Gshift;
    u8 bs = back_surface->format->Bshift;
    u8 as = 0;

    // Find unused 8-bits to use as our alpha channel
    while(as == rs || as == gs || as == bs) 
    {
      as += 8;
    }
    nf.Ashift = as;
    nf.Rshift = rs;
    nf.Gshift = gs;
    nf.Bshift = bs;
  }
  else if( nf.bpp == 16 )
  {
    nf.Rshift = 11;
    nf.Gshift = 5;
    nf.Bshift = 0;
    nf.Ashift = 16;    
    nf.Rprec = 5;
    nf.Gprec = 6;
    nf.Bprec = 5;
    nf.Aprec = 8;
  }

  MDFN_printf(_("BPP: %d\n"), nf.bpp);

  VTReady = NULL;
  VTDRReady = NULL;
  VTLWReady = NULL;

  VTBuffer[0] = new MDFN_Surface( NULL, 
    MDFNGameInfo->fb_width, MDFNGameInfo->fb_height, MDFNGameInfo->fb_width, nf );
  VTBuffer[1] = new MDFN_Surface( NULL, 
    MDFNGameInfo->fb_width, MDFNGameInfo->fb_height, MDFNGameInfo->fb_width, nf );
  VTLineWidths[0] = (MDFN_Rect *)
    calloc( MDFNGameInfo->fb_height, sizeof(MDFN_Rect));
  VTLineWidths[1] = (MDFN_Rect *)
    calloc( MDFNGameInfo->fb_height, sizeof(MDFN_Rect));

  // Set the screen to our back surface
  screen = back_surface;  
}

extern void BlitScreen(MDFN_Surface *msurface, const MDFN_Rect *DisplayRect, const MDFN_Rect *LineWidths);

static void precallback()
{
  VIDEO_SetTrapFilter( wii_trap_filter );

  if( wii_usb_keepalive )
  {
    UsbKeepAlive(); // Attempt to keep the USB drive from sleeping...
  }

  while( !VTReady && GameThreadRun )
  {
    SDL_Delay(1);
  }

  if( VTReady && GameThreadRun )
  {
#if 0
#ifdef WII_NETTRACE
net_print_string( NULL, 0, "DisplayRect: %d, %d, %dx%d, %dx%d, %d, %dx%d\n", 
  VTDRReady->x, VTDRReady->y, VTDRReady->w, VTDRReady->h, VTReady->w, VTReady->h, VTLWReady[0].w, 
  MDFNGameInfo->nominal_width, MDFNGameInfo->nominal_height );
#endif
#endif

    if( lastRect.w != VTDRReady->w || lastRect.h != VTDRReady->h )
    {            
      if( lastRect.w != VTDRReady->w )
      {
        SDL_FillRect( 
          back_surface, 
          NULL, SDL_MapRGB( back_surface->format, 0x0,0x0,0x0 ) );
      }

      // Store last rect values
      lastRect.w = VTDRReady->w; lastRect.h = VTDRReady->h;
   
      Emulator* emu = emuRegistry.getCurrentEmulator();
      int width = ( emu->getRotation() ? VTDRReady->h : VTDRReady->w );
      Rect r;
      emu->getResizeScreenRect( &r );

      if( emu->isDoubleStrikeEnabled() )
      {
        if( ( VTDRReady->h + VTDRReady->y ) > 240 )
        {
          WII_SetInterlaceVideoMode( r.w, r.h, width );
        }
        else
        {        
          WII_SetDoubleStrikeVideoMode( 
            ( emu->getRotation() ? r.w>>1 : r.w ),
            ( emu->getRotation() ? r.h : r.h>>1 ), 
            width );
        }
      }
      else
      {
        if( wii_gx_vi_scaler && !wii_filter )
        {
          // GX + VI scaler
          WII_SetStandardVideoMode( r.w, r.h, width );
        }
        else
        {
          // GX scaler
          emu->resizeScreen(); 
        }
      }
    }

    BlitScreen((MDFN_Surface *)VTReady, (MDFN_Rect *)VTDRReady, (MDFN_Rect*)VTLWReady);
    FPS_IncBlitted();
    VTReady = NULL;
  }
}

extern int RewindState;
static bool lastrewind = false;

/*
 * Enables/disables rewind
 *
 * val  Whether rewind should be enabled
 */
void wii_mednafen_enable_rewind( bool val )
{
  Emulator* emu = emuRegistry.getCurrentEmulator();
  bool rewind = ( val && emu->isRewindSupported() );
  if( rewind != lastrewind )
  {
    lastrewind = rewind;
    RewindState = rewind;
    MDFNI_EnableStateRewind( rewind );    
#ifdef WII_NETTRACE
    net_print_string( NULL, 0, "RewindChanged:%d\n", rewind );
#endif
  }
}

static void update_ports()
{
#ifdef WII_NETTRACE
  net_print_string( NULL, 0, "ResetControls:%d\n", resetControls );
#endif

  if( !resetControls )
  {
    return;
  }

  resetControls = false; // Mark the controls as having been reset

  Emulator* emu = emuRegistry.getCurrentEmulator();
  const char** devices = emu->getInputDevices();

  if( devices != NULL )
  {
    if( lastrewind )
    {
      wii_mednafen_enable_rewind( false );
    }

    KillPortInfo();

    //const char* devices[] = { "gamepad", "gamepad", "gamepad", "gamepad", "none", NULL };
    int port = 0;
printf( "Total ports:%d\n", MDFNGameInfo->InputInfo->InputPorts );
    while( devices[port] != NULL && port < MDFNGameInfo->InputInfo->InputPorts )
    {
      char tmp_setting_name[256];
      trio_snprintf( tmp_setting_name, 256, "%s.input.%s", 
        MDFNGameInfo->shortname, MDFNGameInfo->InputInfo->Types[port].ShortName );    
printf( "Device setting:%s=%s\n", tmp_setting_name, devices[port] );
      MDFNI_SetSetting( tmp_setting_name, devices[port] );

      port++;
    }

    BuildPortInfo( MDFNGameInfo );

    if( lastrewind )
    {
      wii_mednafen_enable_rewind( true );
    }
  }
}

/*
 * The emulation loop
 *
 * resume   Whether we are resuming
 */
void wii_mednafen_emu_loop( BOOL resume )
{
  Emulator* emu = emuRegistry.getCurrentEmulator();
  update_ports();
  wii_mednafen_enable_rewind( wii_rewind ); // Update rewind  

  reset_video();

  emu->getDbManager().applyButtonMap(); // Apply the button map
  emu->onPreLoop();

  if( MDFN_GetSettingB("cheats") != wii_cheats )
  {
    MDFNI_SetSettingB( "cheats", wii_cheats );
  }

  for(int i = 0; i < 2; i++)
    ((MDFN_Surface *)VTBuffer[i])->Fill(0, 0, 0, 0);

  wii_sdl_black_back_surface();

  memset( &lastRect, 0, sizeof(Rect) ); // Clear the last rect
  WII_SetFilter( wii_filter );
  WII_SetRotation( emu->getRotation() * 90 );

  wii_gx_push_callback( &gxrender_callback, TRUE, precallback );   

  ClearSound();
  PauseSound( 0 );

  GameThreadRun = 1;
  NeedVideoChange = 0;
 
  GameLoop( NULL );  

  PauseSound( 1 );

  wii_gx_pop_callback();  
}

#define CB_PIXELSIZE 14
#define CB_H CB_PIXELSIZE
#define CB_PADDING 2
#define CB_X -310
#define CB_Y 196

/*
 * GX render callback
 */
static void gxrender_callback()
{
  static int callback = 0;

  GX_SetVtxDesc( GX_VA_POS, GX_DIRECT );
  GX_SetVtxDesc( GX_VA_CLR0, GX_DIRECT );
  GX_SetVtxDesc( GX_VA_TEX0, GX_NONE );

  Mtx m;    // model matrix.
  Mtx mv;   // modelview matrix.

  guMtxIdentity( m ); 
  guMtxTransApply( m, m, 0, 0, -100 );
  guMtxConcat( gx_view, m, mv );
  GX_LoadPosMtxImm( mv, GX_PNMTX0 ); 

  GXColor color = (GXColor){0x0, 0x0, 0x0, 0x80};                       

  bool doubleStrike =  
    emuRegistry.getCurrentEmulator()->isDoubleStrikeEnabled();

  // Do debug output if we are not using double strike and we are
  // not using the GX+VI scaler or filtering is enabled.
  if( wii_debug && !doubleStrike && ( !wii_gx_vi_scaler || wii_filter ) )
  {    
    static char virtfps[64];
    static char drawnfps[64];
    static char blitfps[64];
    static char defaultText[256] = "";
    static char text[256];

    if( callback++ % 60 == 0 )
    {
      CalcFramerates( virtfps, drawnfps, blitfps, 64 );  
    }

    bool entryLoaded = false;
    Emulator *emu = emuRegistry.getCurrentEmulator();
    if( emu != NULL )
    {
      dbEntry* entry = emu->getDbManager().getEntry();
      entryLoaded = entry != NULL && entry->loaded;
    }
    snprintf( 
      defaultText, sizeof(defaultText), "%s %s %s hash:%s%s", 
      virtfps, drawnfps, blitfps,
      wii_cartridge_hash_with_header, 
      ( entryLoaded ? " (db)" : "" )
    );

    const char* displayText = defaultText;
    if( emu != NULL &&
        emu->updateDebugText( text, defaultText, sizeof(text) ) )
    {
      displayText = text;
    }
    wii_gx_drawrectangle( 
      CB_X + -CB_PADDING, 
      CB_Y + CB_H + CB_PADDING, 
      wii_gx_gettextwidth( CB_PIXELSIZE, (char*)displayText ) + (CB_PADDING<<1), 
      CB_H + (CB_PADDING<<1), 
      color, TRUE );

    wii_gx_drawtext(  
      CB_X, CB_Y, CB_PIXELSIZE, displayText, ftgxWhite, FTGX_ALIGN_BOTTOM );
  }

  if( message_time != 0 )
  {
    if( SDL_GetTicks() > message_time )
    {
      message_time = 0;
    }
    else
    {
      int textWidth = wii_gx_gettextwidth( CB_PIXELSIZE, (char*)message );
      int y = -(doubleStrike ? ( CB_Y >> 1 ) : CB_Y );

      wii_gx_drawrectangle( 
        -(textWidth >> 1) + -CB_PADDING, 
        y + CB_H + CB_PADDING, 
        textWidth + (CB_PADDING<<1), 
        CB_H + (CB_PADDING<<1), 
        (GXColor){0x0, 0x0, 0x0, 0xC0}, TRUE );

#if 0
      wii_gx_drawrectangle( 
        CB_X + -CB_PADDING, 
        -CB_Y + CB_H + CB_PADDING, 
        wii_gx_gettextwidth( CB_PIXELSIZE, (char*)message ) + (CB_PADDING<<1), 
        CB_H + (CB_PADDING<<1), 
        color, TRUE );
#endif

      wii_gx_drawtext( 
        0 /*CB_X*/, y, 
        CB_PIXELSIZE, message, ftgxWhite, 
        FTGX_ALIGN_BOTTOM | FTGX_JUSTIFY_CENTER  );
    }
  }
}

// The current direction of the controls screen alpha
static int controls_alpha_dir = 0;
// The current alpha level for the controls screen
static int controls_alpha = 0;

/*
 * Callback used to display an image indicating the control configuration
 * for the current cartridge
 */
static void controls_render_callback()
{  
  GX_SetVtxDesc( GX_VA_POS, GX_DIRECT );
  GX_SetVtxDesc( GX_VA_CLR0, GX_DIRECT );
  GX_SetVtxDesc( GX_VA_TEX0, GX_NONE );

  Mtx m;    // model matrix.
  Mtx mv;   // modelview matrix.

  guMtxIdentity( m ); 
  guMtxTransApply( m, m, 0, 0, -100 );
  guMtxConcat( gx_view, m, mv );
  GX_LoadPosMtxImm( mv, GX_PNMTX0 ); 

  if( mote_not_supported_idata == NULL )
  {
    mote_not_supported_idata =
      wii_gx_loadimagefrombuff( wiimotenotsupported_png );
  }

  wii_gx_drawimage( 
    -(mote_not_supported_idata->width>>1), 
    (mote_not_supported_idata->height>>1), 
    mote_not_supported_idata->width, 
    mote_not_supported_idata->height, 
    mote_not_supported_idata->data, 
    0, 1.0, 1.0, controls_alpha );
}

/*
 * Displays a screen indicating the controls configuration for the current
 * cartridge
 *
 * return   Returns 1 if we should continue past the screen
 */
int wii_mednafen_show_controls_screen()
{ 
  controls_alpha = controls_alpha_dir = 0;

  WPAD_ScanPads();
  PAD_ScanPads();

  expansion_t exp;
  WPAD_Expansion( 0, &exp );

  if( exp.type == WPAD_EXP_CLASSIC || exp.type == WPAD_EXP_NUNCHUK )
  {
    // The classic or nunchuk is plugged in
    return 1;
  }

  const int ALPHA_INC = 10;
  const int MAX_TIME = 5 * 1000; // 5 seconds

  // Push our callback
  wii_gx_push_callback( &controls_render_callback, FALSE, NULL );  

  u32 startTime = SDL_GetTicks();

  int retVal = 1;
  do
  {
    int nextVal = controls_alpha;
    if( !controls_alpha_dir )
    {
      if( nextVal < 0xff )
      {
        nextVal+=ALPHA_INC;
      }
    }
    else
    {
      if( nextVal > 0 )
      {
        nextVal-=ALPHA_INC;
      }
    }

    if( nextVal >= 0xff )
    {
      nextVal = 0xff;
    }
    else if( nextVal < 0 )
    {
      nextVal = 0;
    }

    controls_alpha = nextVal;

    int pressed = wii_check_button_pressed();
    if( pressed == -1 )
    {
      retVal = 0;
      break;
    }

    if( pressed || 
        ( ( SDL_GetTicks() - startTime ) > MAX_TIME ) )
    {
      controls_alpha_dir = 1;
    }

    VIDEO_WaitVSync();
  }
  while( controls_alpha > 0 );

  // Pop our callback
  wii_gx_pop_callback();

  return retVal;
}
