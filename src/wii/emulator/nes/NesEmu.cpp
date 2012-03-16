#include "main.h"

#include "ControlMacros.h"
#include "Nes.h"
#include "wii_util.h"
#include "wii_input.h"
#include "wii_hw_buttons.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"  
#endif

static const ScreenSize defaultScreenSizes[] = 
{
  { { 320*1*1.25, 240    },  "1x"               },
  { { 320*2*1.25, 240*2  },  "2x/Full screen"   }
};

static const int defaultScreenSizesCount =
  sizeof( defaultScreenSizes ) / sizeof(ScreenSize);

Nes::Nes() : 
  Emulator( "nes", "NES" ),
  m_configManager( *this ),
  m_dbManager( *this ),
  m_menuManager( *this ),
  m_gameGenie( false ),
  m_wiimoteMode( NES_WM_MODE_IR ),
  m_centerAdjust( 0 ),
  m_sensitivity( 0 ),
  m_smoothing( true )
{
  // The emulator screen size
  m_emulatorScreenSize.w = 256;
  m_emulatorScreenSize.h = 256;

  // Set user screen sizes
  int defaultIndex = 1;
  m_screenSize.w = defaultScreenSizes[defaultIndex].r.w; 
  m_screenSize.h = defaultScreenSizes[defaultIndex].r.h; 
}

ConfigManager& Nes::getConfigManager()
{
  return m_configManager;
}

DatabaseManager& Nes::getDbManager()
{
  return m_dbManager;
}

MenuManager& Nes::getMenuManager()
{
  return m_menuManager;
}

extern bool NESIsVSUni;
extern void MDFN_VSUniCoin();
extern MDFNGI *MDFNGameInfo;
extern int FDS_DiskInsert(int oride);
extern int FDS_DiskEject(void);
extern int FDS_DiskSelect(void);
extern bool DNeedRewind;

static int flipdisk = 0;
static bool specialheld = false;

#define MAXDEVICES 5
#define MAXINPUTBYTES 9

static u8 nesPadData[MAXDEVICES][MAXINPUTBYTES];  

#define WIIMOTE_ROLL_SENSITIVITY_LEVELS 11
#define WIIMOTE_ROLL_SENSITIVITY_ADD 5
#define WIIMOTE_ROLL_MIN ( 10.0 )
#define WIIMOTE_ROLL_MAX ( 120.0 )
#define WIIMOTE_ROLL_CENTER_ADJ_INCREMENT 1.0
#define WIIMOTE_ROLL_CENTER_ADJ_MID 10.0

static float smooth[] = { 0, 0, 0, 0, 0, 0, 0 };
static int smoothCount = sizeof(smooth)/sizeof(float);
static int smoothIdx = -1;

float Nes::applySmoothing( float roll )
{
  NesDbEntry* nesEntry = (NesDbEntry*)getDbManager().getEntry();
  if( !getAppliedSmoothing() )
  {
    return roll;
  }

  if( smoothIdx == -1 )
  {
    for( int i = 0; i < smoothCount; i++ )
    {
      smooth[i] == roll;
    }
    smoothIdx = 0;
  }

  smooth[smoothIdx++] = roll;
  if( smoothIdx == smoothCount )
  {
    smoothIdx = 0;
  }

  roll = 0;
  for( int i = 0; i < smoothCount; i++ )
  {
    roll+=smooth[i];
  }

  return roll / (float)smoothCount;
}

int Nes::readWiimoteRoll()
{
  NesDbEntry* nesEntry = (NesDbEntry*)getDbManager().getEntry();
  bool isRoll = NES_WM_MODE_ROLL == getAppliedWiimoteMode();
  int centerAdjust = getAppliedCenterAdjust();
  int sensitivity = getAppliedSensitivity() + WIIMOTE_ROLL_SENSITIVITY_ADD;

  float rollMax, rollMin, rollRange, rollCenter, rollRatio, 
    paddleRange, rollIncrement;

  Rect* lastRect = wii_mednafen_get_last_rect();      

  int TRIGMIN = 0; int TRIGMAX = lastRect->w;
  paddleRange = ((float)TRIGMAX) - TRIGMIN;

  rollIncrement = 
    (WIIMOTE_ROLL_MAX - WIIMOTE_ROLL_MIN)/WIIMOTE_ROLL_SENSITIVITY_LEVELS;

  rollMax = WIIMOTE_ROLL_MIN + 
    ((WIIMOTE_ROLL_SENSITIVITY_LEVELS - sensitivity )
      *rollIncrement);

  rollMin = -rollMax;
 
  rollRange = rollMax - rollMin;
  rollCenter = 
    rollRange / 
      (WIIMOTE_ROLL_CENTER_ADJ_MID - 
          (WIIMOTE_ROLL_CENTER_ADJ_INCREMENT*(abs(centerAdjust))));

  if( centerAdjust < 0 ) 
      rollCenter = -rollCenter;

  rollMax -= rollCenter;
  rollMin -= rollCenter;
  rollRatio = paddleRange / rollRange;

  orient_t orient;
  WPAD_Orientation( WPAD_CHAN_0, &orient );
  float roll = isRoll ? -orient.roll : orient.pitch;

  float oldRoll = roll;
  roll = applySmoothing( roll );

//printf( "%f %f\n", oldRoll, roll );
  
  if( roll > rollMin && roll < rollMax )
  {
    int value = ((int)( ( roll - rollMin ) * rollRatio ));
    int retVal = ( TRIGMAX - ( TRIGMIN + value ) );
    if( retVal < TRIGMIN  ) retVal = TRIGMIN;
    else if( retVal > TRIGMAX  ) retVal = TRIGMAX;
    return retVal;
  }
  else
  {        
    return ( roll > ( -rollCenter ) ) ? TRIGMIN : TRIGMAX ;
  }
}

void Nes::readWiimoteIR( int* x, int *y, bool zeroWhenInvalid )
{
  static int lastx = 0;
  static int lasty = 0;

  Rect* lastRect = wii_mednafen_get_last_rect();      
  WPAD_SetVRes( WPAD_CHAN_0, lastRect->w, lastRect->h );
  WPADData *wd = WPAD_Data( WPAD_CHAN_0 );        
  bool valid = wd->ir.valid;
  int zapx = valid ? wd->ir.x : 0;
  int zapy = valid ? wd->ir.y : 0;

  if( valid )
  {
    lastx = zapx;
    lasty = zapy;
  }

  if( !valid )
  {
    *x = zeroWhenInvalid ? 0 : lastx;
    *y = zeroWhenInvalid ? 0 : lasty;
  }
  else
  {
    *x = zapx;
    *y = zapy;
  }
}

void Nes::readArkanoid( int* x )
{
  NesDbEntry* nesEntry = (NesDbEntry*)getDbManager().getEntry();

  if( NES_WM_MODE_IR == getAppliedWiimoteMode() )
  {
    int y;
    readWiimoteIR( x, &y, false );
  }
  else
  {
    *x = readWiimoteRoll();      
  }
}

void Nes::updateControls( bool isRapid )
{
  WPAD_ScanPads();
  PAD_ScanPads();

  memset( nesPadData, 0x0, sizeof(nesPadData) );

  if( flipdisk )
  {
    switch( flipdisk )
    {
      case 60:
        FDS_DiskEject();
        break;
      case 30:
        FDS_DiskSelect();
        break;
      case 1:
        FDS_DiskInsert(-1);
        break;
    }
    flipdisk--;
  }
  
  StandardDbEntry* entry = (StandardDbEntry*)getDbManager().getEntry();  
  NesDbEntry* nesEntry = (NesDbEntry*)entry;  
  NesDbManager& dbManager = (NesDbManager&)getDbManager();

  bool special = false;

  int gamepadButtonCount = dbManager.getMappableButtonCount( NES_PROFILE_GAMEPADS );  

  for( int c = 0; c < 4; c++ )
  {
    READ_CONTROLS_STATE

    u8 result[MAXINPUTBYTES];
    memset( result, 0x0, sizeof(result) );

    int profile = NES_PROFILE_GAMEPADS;          
    if( c == 0 )
    {
      switch( nesEntry->control )
      {
          case NES_CONTROL_SHADOW:
            profile = NES_PROFILE_SPACE_SHADOW;
            break;
          case NES_CONTROL_ZAPPER_P1:
          case NES_CONTROL_ZAPPER_P2:
            profile = NES_PROFILE_ZAPPER;
            break;
          case NES_CONTROL_ARKANOID_P2:
          case NES_CONTROL_ARKANOID_P5:
            profile = NES_PROFILE_ARKANOID;
            break;
          case NES_CONTROL_OEKAKIDS:
            profile = NES_PROFILE_OEKAKIDS;
            break;
          case NES_CONTROL_HYPERSHOT:
            profile = NES_PROFILE_HYPERSHOT;
            break;
      }
    }
    else if( c == 1 )
    {
      switch( nesEntry->control )
      {
        case NES_CONTROL_HYPERSHOT:
          profile = NES_PROFILE_HYPERSHOT;
          break;
      }
    }

    for( int i = 0; i < dbManager.getMappableButtonCount( profile ); i++ )
    {
      BEGIN_IF_BUTTON_HELD(profile)
        u32 val = dbManager.getMappableButton( profile, i )->button;
        if( val == NES_REWIND )
        {
          special = true;
          if( !specialheld )
          {
            specialheld = true;
            DNeedRewind = true;
          }                    
        }
        else if( val == NES_SPECIAL )
        {          
          special = true;
          if( !specialheld )
          {
            specialheld = true;
            if( NESIsVSUni )
            {
              MDFN_VSUniCoin();
            }
            else if( MDFNGameInfo->GameType == GMT_DISK && !flipdisk )
            {
              flipdisk = 60;
            }
          }                    
        }
        else 
        {
          if( !( val & BTN_RAPID ) || isRapid )
          {
            if( i >= gamepadButtonCount )
            {
              if( profile == NES_PROFILE_ZAPPER || 
                  profile == NES_PROFILE_OEKAKIDS )
              {
                result[8] |= ( val & 0xFF );
              }
              else if( profile == NES_PROFILE_SPACE_SHADOW )
              {
                result[val == NES_ZAP_TRIGGER ? 8 : 0] |= ( val & 0xFF );
              }
              else if( profile == NES_PROFILE_ARKANOID )
              {
                result[4] |= ( val & 0xFF );
              }
              else if( profile == NES_PROFILE_HYPERSHOT )
              {
                if( val == NES_HS_I_RUN )
                {
                  result[1] |= ( c == 0 ? NES_HS_I_RUN : NES_HS_II_RUN );
                }
                else if( val == NES_HS_I_JUMP )
                {
                  result[1] |= ( c == 0 ? NES_HS_I_JUMP : NES_HS_II_JUMP );
                }
              }
            }
            else
            {
              result[0] |= ( val & 0xFF );          
            }
          }
        }
      END_IF_BUTTON_HELD
    }    

    if( profile == NES_PROFILE_HYPERSHOT )
    {
      IF_RIGHT
        result[1]|=(c==0?NES_HS_I_JUMP:NES_HS_II_JUMP);
      IF_LEFT
        result[1]|=(c==0?NES_HS_I_JUMP:NES_HS_II_JUMP);
      IF_UP
        result[1]|=(c==0?NES_HS_I_JUMP:NES_HS_II_JUMP);
      IF_DOWN
        result[1]|=(c==0?NES_HS_I_JUMP:NES_HS_II_JUMP);
    }
    else if( profile == NES_PROFILE_SPACE_SHADOW )
    {
      IF_RIGHT
        result[0]|=NES_RIGHT;
      IF_LEFT
        result[0]|=NES_UP;
      IF_UP
        result[0]|=NES_UP;
      IF_DOWN
        result[0]|=NES_DOWN;
    }
    else
    {
      IF_RIGHT
        result[0]|=NES_RIGHT;
      IF_LEFT
        result[0]|=NES_LEFT;
      IF_UP
        result[0]|=NES_UP;
      IF_DOWN
        result[0]|=NES_DOWN;
    }

    if( nesEntry->control != NES_CONTROL_GAMEPADS && c < 2 )
    {
      if( c == 0 )
      {
        int x, y;
        switch( nesEntry->control )
        {
          case NES_CONTROL_OEKAKIDS:
          case NES_CONTROL_SHADOW:
            readWiimoteIR( &x, &y, true );
            MDFN_en32lsb(&(nesPadData[4][0]),x);
            MDFN_en32lsb(&(nesPadData[4][4]),y);
            nesPadData[4][8] = result[8];
            nesPadData[0][0] |= result[0];        
            break;
          case NES_CONTROL_ZAPPER_P1:
            readWiimoteIR( &x, &y, true );
            MDFN_en32lsb(&(nesPadData[0][0]),x);
            MDFN_en32lsb(&(nesPadData[0][4]),y);
            nesPadData[0][8] = result[8];
            nesPadData[1][0] |= result[0];
            break;
          case NES_CONTROL_ZAPPER_P2:
            readWiimoteIR( &x, &y, true );
            MDFN_en32lsb(&(nesPadData[1][0]),x);
            MDFN_en32lsb(&(nesPadData[1][4]),y);
            nesPadData[1][8] = result[8];
            nesPadData[0][0] |= result[0];
            break;
          case NES_CONTROL_ARKANOID_P2:
            readArkanoid( &x );
            MDFN_en32lsb(&(nesPadData[1][0]),x);
            nesPadData[1][4] = result[4];
            nesPadData[0][0] |= result[0];
            break;
          case NES_CONTROL_ARKANOID_P5:
            readArkanoid( &x );
            MDFN_en32lsb(&(nesPadData[4][0]),x);
            nesPadData[4][4] = result[4];
            nesPadData[0][0] |= result[0];
            break;
          case NES_CONTROL_HYPERSHOT:
            nesPadData[0][0] |= result[0];
            nesPadData[4][0] |= result[1];
            break;
        }
      }
      else
      {
        switch( nesEntry->control )
        {
          case NES_CONTROL_OEKAKIDS:
          case NES_CONTROL_SHADOW:          
          case NES_CONTROL_ZAPPER_P1:
          case NES_CONTROL_ARKANOID_P5:
            nesPadData[1][0] |= result[0];
            break;
          case NES_CONTROL_ZAPPER_P2:
          case NES_CONTROL_ARKANOID_P2:
            nesPadData[0][0] |= result[0];
            break;
          case NES_CONTROL_HYPERSHOT:
            nesPadData[1][0] |= result[0];
            nesPadData[4][0] |= result[1];
            break;
        }
      }
    }
    else
    {
      nesPadData[c][0] |= result[0];
    }    
  }

  if( !special )
  {
    specialheld = false;
    DNeedRewind = false;
  }
}

void Nes::updateInputDeviceData( int device, u8 *data, int size )
{
  if( device < MAXDEVICES )
  {
    for( int i = 0; i < size && i < MAXINPUTBYTES; i++ )
    {
      data[i] = nesPadData[device][i];
    }
  }
}

void Nes::onPostLoad()
{
  flipdisk = 0;
  DNeedRewind = false;
  specialheld = false;
}

bool Nes::updateDebugText( 
  char* output, const char* defaultOutput, int len )
{
  return false;
}

bool Nes::isRotationSupported()
{
  return false;
}

bool Nes::isGameGenieEnabled()
{
  return m_gameGenie;
}

void Nes::setGameGenieEnabled( bool enabled )
{
  m_gameGenie = enabled;
}

u8 Nes::getBpp()
{
  return NES_BPP;
}

const ScreenSize* Nes::getDefaultScreenSizes()
{
  return defaultScreenSizes;
}

int Nes::getDefaultScreenSizesCount()
{
  return defaultScreenSizesCount;
}

const ScreenSize* Nes::getDoubleStrikeScreenSize()
{
  return &defaultScreenSizes[1];
}

bool Nes::isRewindSupported()
{
  return MDFNGameInfo->GameType != GMT_DISK;
}

static const char* devices[MAXDEVICES] = { NULL, NULL, NULL, NULL, NULL };

const char** Nes::getInputDevices()
{
  devices[4] = "none";
  for( int i = 0; i < 4; i++ )
  {
    devices[i] = "gamepad";
  }

  NesDbEntry* entry = (NesDbEntry*)getDbManager().getEntry();
  if( entry->control == NES_CONTROL_ZAPPER_P1 )
  {
    devices[0] = "zapper";
  }
  else if( entry->control == NES_CONTROL_ZAPPER_P2 )
  {
    devices[1] = "zapper";
  }
  else if( entry->control == NES_CONTROL_SHADOW )
  {
    devices[4] = "shadow";
  }
  else if( entry->control == NES_CONTROL_OEKAKIDS )
  {
    devices[4] = "oekakids";
  }
  else if( entry->control == NES_CONTROL_ARKANOID_P2 )
  {
    devices[1] = "arkanoid";
  }
  else if( entry->control == NES_CONTROL_ARKANOID_P5 )
  {
    devices[4] = "arkanoid";
  }
  else if( entry->control == NES_CONTROL_HYPERSHOT )
  {
    devices[4] = "hypershot";
  }

  return devices;
}

void Nes::setWiimoteMode( int mode )
{
  m_wiimoteMode = mode;
}

int Nes::getWiimoteMode()
{
  return m_wiimoteMode;
}

int Nes::getAppliedWiimoteMode()
{
  NesDbEntry* entry = (NesDbEntry*)getDbManager().getEntry();
  if( entry->wiimoteMode == NES_WM_MODE_DEFAULT )
  {
    return getWiimoteMode();
  }

  return entry->wiimoteMode;
}

void Nes::setCenterAdjust( int adj )
{
  m_centerAdjust = adj;
}

int Nes::getCenterAdjust()
{
  return m_centerAdjust;
}

int Nes::getAppliedCenterAdjust()
{
  NesDbEntry* entry = (NesDbEntry*)getDbManager().getEntry();
  if( entry->centerAdjust == NES_CENTER_ADJ_DEFAULT )
  {
    return getCenterAdjust();
  }

  return entry->centerAdjust;  
}

void Nes::setSensitivity( int value )
{
  m_sensitivity = value;
}
int Nes::getSensitivity()
{
  return m_sensitivity;
}

int Nes::getAppliedSensitivity()
{
  NesDbEntry* entry = (NesDbEntry*)getDbManager().getEntry();
  if( entry->sensitivity == NES_SENSITIVITY_DEFAULT )
  {
    return getSensitivity();
  }

  return entry->sensitivity;  
}

void Nes::setSmoothing( bool value )
{
  m_smoothing = value;
}

bool Nes::getSmoothing()
{
  return m_smoothing;
}

bool Nes::getAppliedSmoothing()
{
  NesDbEntry* entry = (NesDbEntry*)getDbManager().getEntry();
  if( entry->smoothing == NES_SMOOTHING_DEFAULT )
  {
    return getSmoothing();
  }

  return entry->smoothing;  
}
