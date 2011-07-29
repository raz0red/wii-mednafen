/* Mednafen - Multi-system Emulator
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "main.h"

#include "video.h"

#include "Emulators.h"

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"  
#endif

SDL_Surface *screen = NULL;

#define COPY_SCREEN                                                \
  int indent = ( msurface->w - DisplayRect->w ) >> 1;              \
  int topindent = ( msurface->h - DisplayRect->h ) >> 1;           \
  int destPitch = screen->pitch/screen->format->BytesPerPixel;     \
  int srcPitch = msurface->pitch32;                                \
  int width = ( msurface->w - ( indent << 1 ) ) *                  \
    screen->format->BytesPerPixel;                                 \
  dest += screen->offset/screen->format->BytesPerPixel +           \
    (destPitch * topindent) + indent;                              \
  src += srcPitch * DisplayRect->y;                                \
  for( int y = 0; y < DisplayRect->h; y++ )                        \
  {                                                                \
    memcpy( dest, src, width );                                    \
    dest+=destPitch;                                               \
    src+=srcPitch;                                                 \
  }

int mednafen_skip_frame = 0;

void BlitScreen(MDFN_Surface *msurface, const MDFN_Rect *DisplayRect, const MDFN_Rect *LineWidths)
{
  if(!screen) return;

  u8 bpp = emuRegistry.getCurrentEmulator()->getBpp();
  switch( bpp )
  {
    case 8:
      {
        u8* dest = (u8*)screen->pixels;
        u8* src = msurface->pixels8;
        COPY_SCREEN
      }
      break;
    case 16:
      {
        u16* dest = (u16*)screen->pixels;
        u16* src = msurface->pixels16;
        COPY_SCREEN
      }
      break;
    default:
      {
        u32* dest = (u32*)screen->pixels;
        u32* src = msurface->pixels;
        COPY_SCREEN
      }
      break;
  }

  SDL_Flip(screen); 
}