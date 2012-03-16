#include "main.h"

#include "ControlMacros.h"
#include "GameBoy.h"
#include "wii_util.h"
#include "wii_input.h"
#include "wii_hw_buttons.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"

static const ScreenSize defaultScreenSizes[] = 
{
  { { 320,        240 },        "1x"           },
  { { 320*2,      240*2 },      "2x"           },
  { { 320*3,      240*3 },      "3x"           }, 
  { { 320*3.138,  240*3.138 },  "Full screen"  }, // default
  { { 320*4,      240*3.138 },  "Fill screen"  }
};

static const int defaultScreenSizesCount =
  sizeof( defaultScreenSizes ) / sizeof(ScreenSize);

GameBoy::GameBoy() : 
  Emulator( "gb", "GameBoy Color" ),
  m_configManager( *this ),
  m_dbManager( *this ),
  m_menuManager( *this )
{
  // The emulator screen size
  m_emulatorScreenSize.w = 160;
  m_emulatorScreenSize.h = 144;

  // Set user screen sizes
  int defaultIndex = 3;
  m_screenSize.w = defaultScreenSizes[defaultIndex].r.w; 
  m_screenSize.h = defaultScreenSizes[defaultIndex].r.h; 

  m_doubleStrike = DOUBLE_STRIKE_DISABLED;
}

ConfigManager& GameBoy::getConfigManager()
{
  return m_configManager;
}

DatabaseManager& GameBoy::getDbManager()
{
  return m_dbManager;
}

MenuManager& GameBoy::getMenuManager()
{
  return m_menuManager;
}

extern bool DNeedRewind;
static bool specialheld = false;

void GameBoy::updateControls( bool isRapid )
{
  bool special = false;

  WPAD_ScanPads();
  PAD_ScanPads();

  int c = 0;
  READ_CONTROLS_STATE

  u16 result = 0;
  StandardDbEntry* entry = (StandardDbEntry*)getDbManager().getEntry();
  StandardDatabaseManager& dbManager = 
    (StandardDatabaseManager&)getDbManager();

  for( int i = 0; i < dbManager.getMappableButtonCount( entry->profile ); i++ )
  {
    BEGIN_IF_BUTTON_HELD(entry->profile)
      u32 val = GameBoyDbManager::GB_BUTTONS[ i ].button;
      if( val == GB_REWIND )
      {
        special = true;
        if( !specialheld )
        {
          specialheld = true;
          DNeedRewind = true;
        }                    
      }
      else if( !( val & BTN_RAPID ) || isRapid )
      {
        result |= ( val & 0xFFFF );
      }
    END_IF_BUTTON_HELD
  }    

  IF_RIGHT
    result|=GB_RIGHT;
  IF_LEFT
    result|=GB_LEFT;
  IF_UP
    result|=GB_UP;
  IF_DOWN
    result|=GB_DOWN;

  m_padData[0] = result;

  if( !special )
  {
    specialheld = false;
    DNeedRewind = false;
  }
}

void GameBoy::onPostLoad()
{
  specialheld = false;
  DNeedRewind = false;
}

bool GameBoy::updateDebugText( 
  char* output, const char* defaultOutput, int len )
{
  return false;
}

bool GameBoy::isRotationSupported()
{
  return false;
}

const ScreenSize* GameBoy::getDefaultScreenSizes()
{
  return defaultScreenSizes;
}

int GameBoy::getDefaultScreenSizesCount()
{
  return defaultScreenSizesCount;
}

const ScreenSize* GameBoy::getDoubleStrikeScreenSize()
{
  return &defaultScreenSizes[1];
}

bool GameBoy::isRewindSupported()
{
  return true;
}
