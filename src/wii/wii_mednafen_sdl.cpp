/*
WiiMednafen : Wii port of the Mednafen emulator

Copyright (C) 2011 raz0red
*/

#include "wii_sdl.h"

#include "wii_mednafen.h"

// The original 8bpp palette
SDL_Palette orig_8bpp_palette;
SDL_Color orig_8bpp_colors[512];

#if 0
extern "C" void WII_SetWidescreen(int wide);
#endif

/*
 * Initializes the SDL
 */
int wii_sdl_handle_init()
{
  if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0) 
  {
    return 0;
  }

  if( SDL_InitSubSystem( SDL_INIT_VIDEO ) < 0 ) 
  {
    return 0;
  }

  back_surface = 
    SDL_SetVideoMode( 640, 480, 8,
      SDL_HWSURFACE | SDL_HWPALETTE | SDL_FULLSCREEN );

  if( !back_surface) 
  {
    return 0;
  }

  int ncolors = back_surface->format->palette->ncolors;
  memcpy( orig_8bpp_colors, 
    back_surface->format->palette->colors, 
    ncolors * sizeof(SDL_Color) );
  orig_8bpp_palette.colors = orig_8bpp_colors;
  orig_8bpp_palette.ncolors = ncolors;

  return 1;
}
