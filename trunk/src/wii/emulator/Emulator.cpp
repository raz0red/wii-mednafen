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
  m_padData( 0 )
{
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

u16 Emulator::getPadData()
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

float Emulator::getCurrentScreenSizeRatio()
{  
  return isMultiRes() ? 
    (float)m_baseMultiResScreenSize.w / (float)VTDRReady->w :
    1.0;
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
        float ratio = getCurrentScreenSizeRatio();
        WII_ChangeSquare( 
          m_screenSize.w * ratio, m_screenSize.h, 0, 0 );
        m_lastSize.w = VTDRReady->w;
        m_lastSize.h = VTDRReady->h;
#ifdef WII_NETTRACE
net_print_string( NULL, 0, 
  "resizing to: w:%dx%d, ratio:%f\n", VTDRReady->w, VTDRReady->h, ratio );
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

u8 Emulator::getBpp()
{
  return 32;
}

