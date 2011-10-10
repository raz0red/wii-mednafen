#include "main.h"
#include "Emulator.h"
#include "DatabaseManager.h"
#include "wii_mednafen_main.h"

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"  
#endif

extern volatile MDFN_Rect *VTDRReady;
extern volatile MDFN_Surface *VTReady;

extern "C" 
{
void WII_SetRotation( int rot );
void WII_ChangeSquare(int xscale, int yscale, int xshift, int yshift);
}

Emulator::Emulator( const char* key, const char* name ) : 
  m_key( key ),
  m_name( name ),
  m_frameSkip( true )
{
  memset( m_padData, 0, sizeof(u32)<<2 );
}

Rect* Emulator::getScreenSize()
{
  return &m_screenSize;
}

Rect* Emulator::getDefaultScreenSize()
{
  return &m_defaultScreenSize;
}

Rect* Emulator::getRotatedScreenSize()
{
  return &m_rotatedScreenSize;
}

Rect* Emulator::getDefaultRotatedScreenSize()
{
  return &m_defaultRotatedScreenSize;
}

Rect* Emulator::getEmulatorScreenSize()
{
  return &m_emulatorScreenSize;
}

Rect* Emulator::getBaseMultiResSize()
{
  if( isMultiRes() )
  {
    return &m_baseMultiResScreenSize;
  }
  return getEmulatorScreenSize();
}

const char* Emulator::getKey()
{
  return m_key;
}

const char* Emulator::getName()
{
  return m_name;
}

u16* Emulator::getPadData()
{
  return m_padData;
}

void Emulator::onPostLoad()
{
}

bool Emulator::onShowControlsScreen()
{
  dbEntry* entry = getDbManager().getEntry();
  if( entry->wiimoteSupported )
  {
    // Wiimote is supported
    return true;
  }

  return wii_mednafen_show_controls_screen();
}

void Emulator::onPreLoop()
{
}

bool Emulator::updateDebugText( 
  char* output, const char* defaultOutput, int len )
{
  return false;
}

bool Emulator::isRotationSupported()
{
  return false;
}

int Emulator::getRotation()
{
  return MDFN_ROTATE0;
}

bool Emulator::isMultiRes()
{
  return false;
}

void Emulator::getCurrentScreenSizeRatio( float* ratiox, float* ratioy )
{  
  *ratiox =
    isMultiRes() ? 
      (float)m_baseMultiResScreenSize.w / (float)VTDRReady->w :
      1.0;

  *ratioy = 1.0;
#if 0
    isMultiRes() ? 
      (float)m_baseMultiResScreenSize.h / (float)VTDRReady->h :
      1.0;
#endif
}

void Emulator::resizeScreen( bool force )
{  
  if( isMultiRes() )
  {
    // Multi-res doesn't currently support rotation...
    if( force )
    {
      m_lastSize.w = m_lastSize.h = 0;
      WII_ChangeSquare( m_screenSize.w, m_screenSize.h, 0, 0 );
    }
    else
    {
      if( VTReady && ( VTDRReady->w != m_lastSize.w ) )
      {
        float ratiox, ratioy;
        getCurrentScreenSizeRatio( &ratiox, &ratioy );
        WII_ChangeSquare( 
          m_screenSize.w * ratiox, m_screenSize.h * ratioy, 0, 0 );
        m_lastSize.w = VTDRReady->w;
        m_lastSize.h = VTDRReady->h;
#ifdef WII_NETTRACE
net_print_string( NULL, 0, 
  "resizing to: w:%dx%d, ratio:%f,%f\n", VTDRReady->w, VTDRReady->h, ratiox, ratioy );
#endif
      }
    }
  }
  else
  {
    Rect* screenSize = getRotation() ?
      getRotatedScreenSize() : getScreenSize();
    WII_ChangeSquare( screenSize->w, screenSize->h, 0, 0 );
  }

  WII_SetRotation( getRotation() * 90 );
}

bool Emulator::getFrameSkip()
{
  return m_frameSkip;
}

void Emulator::setFrameSkip( bool skip )
{
  m_frameSkip = skip;
}

bool Emulator::getAppliedFrameSkip()
{
   dbEntry* entry = getDbManager().getEntry();
   if( entry->frameSkip == FRAME_SKIP_DEFAULT )
   {
     return getFrameSkip();
   }
   
   return entry->frameSkip == FRAME_SKIP_ENABLED;
}

u8 Emulator::getBpp()
{
  return 32;
}

