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

static void ScaleLineAvg8(u8 *Target, u8 *Source, int SrcWidth, int TgtWidth, float threshold);
static void ScaleLineAvg16(u16 *Target, u16 *Source, int SrcWidth, int TgtWidth, float threshold);
static void ScaleLineAvg32(u32 *Target, u32 *Source, int SrcWidth, int TgtWidth, float threshold);

#define COPY_SCREEN                                                 \
  int indent = ( screen->w - DisplayRect->w ) >> 1;                 \
  int topindent = ( screen->h - DisplayRect->h ) >> 1;              \
  int destPitch = screen->pitch/screen->format->BytesPerPixel;      \
  int srcPitch = msurface->pitch32;                                 \
  int width = DisplayRect->w * screen->format->BytesPerPixel;       \
  int bpp = screen->format->BytesPerPixel;                          \
  dest += screen->offset/screen->format->BytesPerPixel +            \
  (destPitch * topindent) + indent;                                 \
  src += srcPitch * DisplayRect->y + DisplayRect->x;                \
  if( LineWidths[0].w == ~0 )                                       \
  {                                                                 \
    for( int y = 0; y < DisplayRect->h; y++ )                       \
    {                                                               \
      memcpy( dest, src, width );                                   \
      dest+=destPitch;                                              \
      src+=srcPitch;                                                \
    }                                                               \
  }                                                                 \
  else                                                              \
  {                                                                 \
    for( int y = DisplayRect->y;                                    \
         y < ( DisplayRect->h + DisplayRect->y ); y++ )             \
    {                                                               \
      if( LineWidths[y].w != DisplayRect->w )                       \
      {                                                             \
        scale( dest, src + LineWidths[y].x,                         \
            LineWidths[y].w, DisplayRect->w, 0.5f );                \
      }                                                             \
      else                                                          \
      {                                                             \
        memcpy( dest, src, width );                                 \
      }                                                             \
      dest+=destPitch;                                              \
      src+=srcPitch;                                                \
    }                                                               \
  }

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
      void (*scale)(u8*,u8*,int,int,float) = ScaleLineAvg8;
      COPY_SCREEN
    }
    break;
  case 16:
    {
      u16* dest = (u16*)screen->pixels;
      u16* src = msurface->pixels16;
      void (*scale)(u16*,u16*,int,int,float) = ScaleLineAvg16;
      COPY_SCREEN
    }
    break;
  default:
    {
      u32* dest = (u32*)screen->pixels;
      u32* src = msurface->pixels;
      void (*scale)(u32*,u32*,int,int,float) = ScaleLineAvg32;
      COPY_SCREEN
    }
    break;
  }

  SDL_Flip(screen); 
}

//
// !!! SUPER LAME HACK !!! 
// This is just temporary until I spend time to address the fact that PCE
// can change resolution mid-frame.
//

#define average(a, b)  (((a)+(b))>>1)
void ScaleLineAvg8(u8 *Target, u8 *Source, int SrcWidth, int TgtWidth, float threshold)
{
  int NumPixels = TgtWidth;						
  int IntPart = SrcWidth / TgtWidth;	
  int FractPart = SrcWidth % TgtWidth;
  int Mid = TgtWidth * threshold;	    
  int E = 0;									        
  int skip;
  u8 p;
  skip = (TgtWidth < SrcWidth) ? 0 : TgtWidth / (2*SrcWidth) + 1;
  NumPixels -= skip;
  while (NumPixels-- > 0) {
    p = *Source;		
    if (E >= Mid)		
      p = (u8)average(p, *(Source+1));
    *Target++ = p;	
    Source += IntPart;
    E += FractPart;		
    if (E >= TgtWidth) {
      E -= TgtWidth;		
      Source++;					
    } 
  } 
  while (skip-- > 0)	
    *Target++ = *Source;
}

void ScaleLineAvg16(u16 *Target, u16 *Source, int SrcWidth, int TgtWidth, float threshold)
{
  int NumPixels = TgtWidth;					
  int IntPart = SrcWidth / TgtWidth;
  int FractPart = SrcWidth % TgtWidth;
  int Mid = TgtWidth * threshold;	    
  int E = 0;									        
  int skip;			
  u16 p;				
  skip = (TgtWidth < SrcWidth) ? 0 : TgtWidth / (2*SrcWidth) + 1;
  NumPixels -= skip;
  while (NumPixels-- > 0) {
    p = *Source;								
    if (E >= Mid)								
      p = (u16)average(p, *(Source+1));		
    *Target++ = p;						
    Source += IntPart;				
    E += FractPart;						
    if (E >= TgtWidth) {			
      E -= TgtWidth;					
      Source++;								
    }
  } 
  while (skip-- > 0)							
    *Target++ = *Source;					
}

void ScaleLineAvg32(u32 *Target, u32 *Source, int SrcWidth, int TgtWidth, float threshold)
{
  int NumPixels = TgtWidth;						
  int IntPart = SrcWidth / TgtWidth;	  
  int FractPart = SrcWidth % TgtWidth;	
  int Mid = TgtWidth * threshold;	      
  int E = 0;									          
  int skip;	
  u32 p;		
  skip = (TgtWidth < SrcWidth) ? 0 : TgtWidth / (2*SrcWidth) + 1;
  NumPixels -= skip;
  while (NumPixels-- > 0) {
    p = *Source;					
    if (E >= Mid)					
      p = (u32)average(p, *(Source+1));	
    *Target++ = p;					
    Source += IntPart;			
    E += FractPart;					
    if (E >= TgtWidth) {		
      E -= TgtWidth;				
      Source++;							
    }
  }
  while (skip-- > 0)			
    *Target++ = *Source;	
}
