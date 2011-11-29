#include "main.h"

#include "WonderSwan.h"
#include "wii_util.h"
#include "wii_input.h"
#include "wii_hw_buttons.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"

extern "C" 
{
void WII_SetRotation( int rot );
}


static const ScreenSize defaultScreenSizes[] = 
{
  { { 320,         240        },  "1x"          },
  { { 320*2,       240*2      },  "2x"          },
  { { 320*2.5,     240*2.5    },  "2.5x"        },
  { { 320*2.86,    240*2.86   },  "Full screen" },
  { { 320*2.86,    240*3.138  },  "Fill screen" }
};

static const int defaultScreenSizesCount =
  sizeof( defaultScreenSizes ) / sizeof(ScreenSize);

static const ScreenSize defaultRotatedScreenSizes[] = 
{
  { { 320,        240       }, "1x"              },
  { { 320*2,      240*2     }, "2x/Full screen"  },
  { { 320*2,      240*4.444 }, "Fill screen"     }
};

static const int defaultRotatedScreenSizesCount =
  sizeof( defaultRotatedScreenSizes ) / sizeof(ScreenSize);

WonderSwan::WonderSwan() : 
  Emulator( "wswan", "WonderSwan" ),
  m_configManager( *this ),
  m_dbManager( *this ),
  m_menuManager( *this )
{
  // The emulator screen size
  m_emulatorScreenSize.w = 224;
  m_emulatorScreenSize.h = 144;

  // Set user screen sizes
  int defaultIndex = 3;
  m_screenSize.w = defaultScreenSizes[defaultIndex].r.w; 
  m_screenSize.h = defaultScreenSizes[defaultIndex].r.h; 

  defaultIndex = 1;
  m_rotatedScreenSize.w = 
    defaultRotatedScreenSizes[defaultIndex].r.w; 
  m_rotatedScreenSize.h = 
    defaultRotatedScreenSizes[defaultIndex].r.h; 

  m_doubleStrike = DOUBLE_STRIKE_DISABLED;
}

ConfigManager& WonderSwan::getConfigManager()
{
  return m_configManager;
}

DatabaseManager& WonderSwan::getDbManager()
{
  return m_dbManager;
}

MenuManager& WonderSwan::getMenuManager()
{
  return m_menuManager;
}

static bool specialheld = false;

void WonderSwan::updateControls( bool isRapid )
{
  bool special = false;

  WPAD_ScanPads();
  PAD_ScanPads();

  // Check the state of the controllers
  u32 pressed = WPAD_ButtonsDown( 0 );
  u32 held = WPAD_ButtonsHeld( 0 );  
  u32 gcPressed = PAD_ButtonsDown( 0 );
  u32 gcHeld = PAD_ButtonsHeld( 0 );

  // Classic or Nunchuck?
  expansion_t exp;
  WPAD_Expansion( 0, &exp );          

  BOOL isClassic = ( exp.type == WPAD_EXP_CLASSIC );
  BOOL isNunchuk = ( exp.type == WPAD_EXP_NUNCHUK );

  // Mask off the Wiimote d-pad depending on whether a nunchuk
  // is connected. (Wiimote d-pad is left when nunchuk is not
  // connected, right when it is).
  u32 heldLeft = ( isNunchuk ? ( held & ~0x0F00 ) : held );
  u32 heldRight = ( !isNunchuk ? ( held & ~0x0F00 ) : held );

  // Analog for Wii controls
  float expX = wii_exp_analog_val( &exp, TRUE, FALSE );
  float expY = wii_exp_analog_val( &exp, FALSE, FALSE );
  float expRX = isClassic ? wii_exp_analog_val( &exp, TRUE, TRUE ) : 0;
  float expRY = isClassic ? wii_exp_analog_val( &exp, FALSE, TRUE ) : 0;

  // Analog for Gamecube controls
  s8 gcX = PAD_StickX( 0 );
  s8 gcY = PAD_StickY( 0 );
  s8 gcRX = PAD_SubStickX( 0 );
  s8 gcRY = PAD_SubStickY( 0 );

  // Check for home
  if( ( pressed & WII_BUTTON_HOME ) ||
    ( gcPressed & GC_BUTTON_HOME ) ||
    wii_hw_button )
  {
    GameThreadRun = 0;
  }

  u16 result = 0;

  //
  // Mapped buttons
  //
  
  StandardDbEntry* entry = (StandardDbEntry*)getDbManager().getEntry();

  for( int i = 0; i < WS_BUTTON_COUNT; i++ )
  {
    if( ( held &
          ( ( isClassic ? 
                entry->appliedButtonMap[ 
                  WII_CONTROLLER_CLASSIC ][ i ] : 0 ) |
            ( isNunchuk ?
                entry->appliedButtonMap[
                  WII_CONTROLLER_CHUK ][ i ] :
                entry->appliedButtonMap[
                  WII_CONTROLLER_MOTE ][ i ] ) ) ) ||
        ( gcHeld &
            entry->appliedButtonMap[
              WII_CONTROLLER_CUBE ][ i ] ) )
    {
      u32 val = WonderSwanDbManager::WS_BUTTONS[ i ].button;
      if( val == WS_ROTATE )
      {        
        special = true;
        if( !specialheld )
        {
          specialheld = true;

          // Swap between the two profiles (normal and rotated)
          entry->profile ^= 1;
          WII_SetRotation( entry->profile * 90 ); // Rotate screen
          wii_mednafen_reset_last_rect(); // Clear last rect cache (reset)

          // Update the button mappings
          WonderSwanDbManager& dbManager = 
            (WonderSwanDbManager&)getDbManager();
          //dbManager.resetButtons();  // TODO:MULTIPROF 
          dbManager.applyButtonMap();
        }     
      }
      else if( !( val & BTN_RAPID ) || isRapid )
      {
        result |= ( val & 0xFFFF );
      }
    }
  }    

  //
  // Left sticks and pads
  //
  bool rot = ( entry->profile == 1 );

  if( wii_digital_right( !isNunchuk, isClassic, heldLeft ) ||
      ( gcHeld & GC_BUTTON_RIGHT ) ||
      wii_analog_right( expX, gcX ) )
    result|=( rot ? WS_Y3 : WS_X2 );

  if( wii_digital_left( !isNunchuk, isClassic, heldLeft ) || 
      ( gcHeld & GC_BUTTON_LEFT ) ||                       
      wii_analog_left( expX, gcX ) )
    result|=( rot ? WS_Y1 : WS_X4 );

  if( wii_digital_up( !isNunchuk, isClassic, heldLeft ) || 
      ( gcHeld & GC_BUTTON_UP ) ||
      wii_analog_up( expY, gcY ) )
    result|=( rot ? WS_Y2 : WS_X1 );

  if( wii_digital_down( !isNunchuk, isClassic, heldLeft ) ||
      ( gcHeld & GC_BUTTON_DOWN ) ||
      wii_analog_down( expY, gcY ) )
    result|=( rot ? WS_Y4 : WS_X3 );

  m_padData[0] = result;

  if( !special )
  {
    specialheld = false;
  }
}

void WonderSwan::onPostLoad()
{
}

bool WonderSwan::updateDebugText( 
  char* output, const char* defaultOutput, int len )
{
  return false;
}

bool WonderSwan::isRotationSupported()
{
  return true;
}

int WonderSwan::getRotation()
{
  return 
    ((WswanDbEntry*)m_dbManager.getEntry())->base.profile;
}

u8 WonderSwan::getBpp()
{
  return WSWAN_BPP;
}

const ScreenSize* WonderSwan::getDefaultScreenSizes()
{
  return defaultScreenSizes;
}

int WonderSwan::getDefaultScreenSizesCount()
{
  return defaultScreenSizesCount;
}

const ScreenSize* WonderSwan::getDefaultRotatedScreenSizes()
{
  return defaultRotatedScreenSizes;
}

int WonderSwan::getDefaultRotatedScreenSizesCount()
{
  return defaultRotatedScreenSizesCount;
}

const ScreenSize* WonderSwan::getDoubleStrikeScreenSize()
{
  return &defaultScreenSizes[1];
}

const ScreenSize* WonderSwan::getDoubleStrikeRotatedScreenSize()
{
  return &defaultRotatedScreenSizes[1];
}

