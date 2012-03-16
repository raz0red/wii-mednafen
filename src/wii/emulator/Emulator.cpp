#include "main.h"
#include "Emulator.h"
#include "DatabaseManager.h"
#include "wii_mednafen.h"
#include "wii_mednafen_main.h"

#ifdef WII_NETTRACE
#include <network.h> 
#include "net_print.h"  
#endif
  
extern volatile MDFN_Rect *VTDRReady;
extern volatile MDFN_Surface *VTReady;
extern MDFNGI *MDFNGameInfo;

extern uint32 round_up_pow2(uint32 v);

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
  m_doubleStrike = DOUBLE_STRIKE_DEFAULT;
  m_volume = VOLUME_DEFAULT;
}

Rect* Emulator::getScreenSize()
{
  return &m_screenSize;
}

Rect* Emulator::getRotatedScreenSize()
{
  return &m_rotatedScreenSize;
}

Rect* Emulator::getEmulatorScreenSize()
{
  return &m_emulatorScreenSize;
}

const char* Emulator::getKey()
{
  return m_key;
}

const char* Emulator::getName()
{
  return m_name;
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

void Emulator::getCurrentScreenSizeRatio( float* ratiox, float* ratioy )
{   
  *ratiox = (float)MDFNGameInfo->nominal_width/(float)VTDRReady->w;
  *ratioy = (float)MDFNGameInfo->nominal_height/(float)VTDRReady->h;
}

void Emulator::getResizeScreenRect( Rect* rect )
{ 
  float ratiox, ratioy;
  getCurrentScreenSizeRatio( &ratiox, &ratioy );

  if( isDoubleStrikeEnabled() )
  {
    const ScreenSize* size = 
      getRotation() ? 
        getDoubleStrikeRotatedScreenSize() :
        getDoubleStrikeScreenSize();
      
    rect->w = (float)size->r.w * ratiox;
    rect->h = (float)size->r.h * ratioy; 
  }
  else
  {
    if( getRotation() )
    {
      rect->w = (float)m_rotatedScreenSize.w * ratiox;
      rect->h = (float)m_rotatedScreenSize.h * ratioy; 
    }
    else
    {
      rect->w = (float)m_screenSize.w * ratiox;
      rect->h = (float)m_screenSize.h * ratioy; 
    }
  }
#ifdef WII_NETTRACE
    net_print_string( NULL, 0, "resizing to: w:%dx%d, ratio:%f,%f, rect->w:%d, rect->h:%d\n", 
  VTDRReady->w, VTDRReady->h, ratiox, ratioy, rect->w, rect->h );
#endif

  if( wii_16_9_correction )
  {
    if( getRotation() )
    {
      rect->h = ( rect->h * 3 ) / 4;
    }
    else
    {
      rect->w = ( rect->w * 3 ) / 4;
    }
  }

  rect->w = ((rect->w+1)&~1);
  rect->h = ((rect->h+1)&~1);
}

void Emulator::resizeScreen() 
{
  Rect r;
  getResizeScreenRect( &r );
  WII_ChangeSquare( r.w, r.h, 0, 0 );
#ifdef WII_NETTRACE
  net_print_string( NULL, 0, 
    "resizeScreen, change square: %dx%d\n", r.w, r.h );
#endif 
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

bool Emulator::isDoubleStrikeSupported()
{
  return true; 
}
   
u8 Emulator::getBpp()
{ 
  return 32;
}

const ScreenSize* Emulator::getDefaultScreenSizes()
{
  return NULL;
}

int Emulator::getDefaultScreenSizesCount()
{
  return 0;
}
const char* Emulator::getScreenSizeName()
{
  return getScreenSizeName( m_screenSize.w, m_screenSize.h );
}

const char* Emulator::getScreenSizeName( int w, int h )
{
  const ScreenSize* sizes = getDefaultScreenSizes();
  for( int i = 0; i < getDefaultScreenSizesCount(); i++ )
  {
    if( sizes[i].r.w == w && sizes[i].r.h == h )
    {
      return sizes[i].name;
    }
  }

  return "Custom";
}

const ScreenSize* Emulator::getDefaultRotatedScreenSizes()
{
  return NULL;
}

int Emulator::getDefaultRotatedScreenSizesCount()
{
  return 0;
}

const char* Emulator::getRotatedScreenSizeName()
{
  return getRotatedScreenSizeName( 
    m_rotatedScreenSize.w, m_rotatedScreenSize.h );
}

const char* Emulator::getRotatedScreenSizeName( int w, int h )
{
  const ScreenSize* sizes = getDefaultRotatedScreenSizes();
  for( int i = 0; i < getDefaultRotatedScreenSizesCount(); i++ )
  {
    if( sizes[i].r.w == w && sizes[i].r.h == h )
    {
      return sizes[i].name;
    }
  }

  return "Custom";
}

const ScreenSize* Emulator::getDoubleStrikeScreenSize()
{
  return NULL;
}

const ScreenSize* Emulator::getDoubleStrikeRotatedScreenSize()
{
  return NULL;
}

void Emulator::setDoubleStrikeMode( int mode )
{
  m_doubleStrike = mode;  
}

int Emulator::getDoubleStrikeMode()
{
  return m_doubleStrike;
}

bool Emulator::isDoubleStrikeEnabled()
{
  return isDoubleStrikeSupported() && 
    ( ( m_doubleStrike == DOUBLE_STRIKE_ENABLED ) ||
      ( ( m_doubleStrike == DOUBLE_STRIKE_DEFAULT ) &&    
        wii_double_strike_mode ) );
}

int Emulator::getVolume()
{
  return m_volume;
}
void Emulator::setVolume( int volume )
{
  m_volume = volume;
}

int Emulator::getAppliedVolume()
{
  if( m_volume == VOLUME_DEFAULT )
  {
    return wii_volume;
  }
  else
  {
    return m_volume;
  }
}

bool Emulator::isRewindSupported()
{
  return false;
}

const char** Emulator::getInputDevices()
{
  return NULL;
}

void Emulator::updateInputDeviceData( int device, u8 *data, int size )
{
  if( device < 4 )
  {
    u16 padData = m_padData[device];
    if( size >= 1 )
    {
      data[0] = padData & 0xFF;
    }
    if( size >= 2 )
    {
      data[1] = ( padData >> 8 ) & 0xFF;    
    }
  }
}