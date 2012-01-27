#include "video-common.h"
#include "Deinterlacer.h"

Deinterlacer::Deinterlacer()
{
  FieldBuffer = NULL;
}

Deinterlacer::~Deinterlacer()
{
  if(FieldBuffer)
  {
    delete FieldBuffer;
    FieldBuffer = NULL;
  }
}

void Deinterlacer::Process(MDFN_Surface *surface, const MDFN_Rect &DisplayRect, MDFN_Rect *LineWidths, const bool field)
{
  if(!FieldBuffer || FieldBuffer->w < surface->w || FieldBuffer->h < (surface->h / 2))
  {
    if(FieldBuffer)
      delete FieldBuffer;

    FieldBuffer = new MDFN_Surface(NULL, surface->w, surface->h / 2, surface->w, surface->format);
    LWBuffer.resize(FieldBuffer->h);
  }

  for(int y = 0; y < DisplayRect.h / 2; y++)
  {
    if( surface->format.bpp == 16 )
    {
      if(StateValid && PrevHeight == DisplayRect.h)
      {
        const uint16 *src = FieldBuffer->pixels16 + y * FieldBuffer->pitch32;
        uint16 *dest = surface->pixels16 + ((y * 2) + (field ^ 1) + DisplayRect.y) * surface->pitch32;
        MDFN_Rect *dest_lw = &LineWidths[(y * 2) + (field ^ 1) + DisplayRect.y];

        dest_lw->x = 0;
        dest_lw->w = LWBuffer[y];

        memcpy(dest, src, LWBuffer[y] * sizeof(uint16));
      }
      else
      {
        const MDFN_Rect *src_lw = (LineWidths[0].w == ~0) ? &DisplayRect : &LineWidths[(y * 2) + field + DisplayRect.y];
        const uint16 *src = surface->pixels16 + ((y * 2) + field + DisplayRect.y) * surface->pitch32 + src_lw->x;
        const int32 dly = ((y * 2) + (field + 1) + DisplayRect.y);
        uint16 *dest = surface->pixels16 + dly * surface->pitch32;

        if(y == 0 && field)
        {
          uint16 black = surface->MakeColor(0, 0, 0);

          LineWidths[dly - 2] = *src_lw;
          memset(&surface->pixels16[(dly - 2) * surface->pitch32], black, src_lw->w * sizeof(uint16));
        }

        if(dly < (DisplayRect.y + DisplayRect.h))
        {
          LineWidths[dly] = *src_lw;
          memcpy(dest, src, src_lw->w * sizeof(uint16));
        }
      }

      //
      //
      //
      //
      //
      //
      {
        const MDFN_Rect *src_lw = (LineWidths[0].w == ~0) ? &DisplayRect : &LineWidths[(y * 2) + field + DisplayRect.y];
        const uint16 *src = surface->pixels16 + ((y * 2) + field + DisplayRect.y) * surface->pitch32 + src_lw->x;
        uint16 *dest = FieldBuffer->pixels16 + y * FieldBuffer->pitch32;

        memcpy(dest, src, src_lw->w * sizeof(uint16));
        LWBuffer[y] = src_lw->w;
      }
    }
    else
    {
      if(StateValid && PrevHeight == DisplayRect.h)
      {
        const uint32 *src = FieldBuffer->pixels + y * FieldBuffer->pitch32;
        uint32 *dest = surface->pixels + ((y * 2) + (field ^ 1) + DisplayRect.y) * surface->pitch32;
        MDFN_Rect *dest_lw = &LineWidths[(y * 2) + (field ^ 1) + DisplayRect.y];

        dest_lw->x = 0;
        dest_lw->w = LWBuffer[y];

        memcpy(dest, src, LWBuffer[y] * sizeof(uint32));
      }
      else
      {
        const MDFN_Rect *src_lw = (LineWidths[0].w == ~0) ? &DisplayRect : &LineWidths[(y * 2) + field + DisplayRect.y];
        const uint32 *src = surface->pixels + ((y * 2) + field + DisplayRect.y) * surface->pitch32 + src_lw->x;
        const int32 dly = ((y * 2) + (field + 1) + DisplayRect.y);
        uint32 *dest = surface->pixels + dly * surface->pitch32;

        if(y == 0 && field)
        {
          uint32 black = surface->MakeColor(0, 0, 0);

          LineWidths[dly - 2] = *src_lw;
          memset(&surface->pixels[(dly - 2) * surface->pitch32], black, src_lw->w * sizeof(uint32));
        }

        if(dly < (DisplayRect.y + DisplayRect.h))
        {
          LineWidths[dly] = *src_lw;
          memcpy(dest, src, src_lw->w * sizeof(uint32));
        }
      }

      //
      //
      //
      //
      //
      //
      {
        const MDFN_Rect *src_lw = (LineWidths[0].w == ~0) ? &DisplayRect : &LineWidths[(y * 2) + field + DisplayRect.y];
        const uint32 *src = surface->pixels + ((y * 2) + field + DisplayRect.y) * surface->pitch32 + src_lw->x;
        uint32 *dest = FieldBuffer->pixels + y * FieldBuffer->pitch32;

        memcpy(dest, src, src_lw->w * sizeof(uint32));
        LWBuffer[y] = src_lw->w;
      }
    }
  }

  PrevHeight = DisplayRect.h;
  StateValid = true;

  if(DisplayRect.y > 0)
    LineWidths[0].w = 0;
}


void Deinterlacer::ClearState(void)
{
  StateValid = false;
  PrevHeight = 0;
}
