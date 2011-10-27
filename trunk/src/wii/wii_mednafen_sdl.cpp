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
