#include "main.h"

#include "ControlMacros.h"
#include "GameGear.h"
#include "wii_util.h"
#include "wii_input.h"
#include "wii_hw_buttons.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"

static const ScreenSize defaultScreenSizes[] = 
{
  { { 320*1*1.275, 240                },  "1x"          },
  { { 320*2*1.275, 240*2              },  "2x"          },
  { { 320*3*1.275, 240*3              },  "3x"          },
  { { 320*4,       240*4*(1.0/1.275)  },  "Full screen" }  // default
};

static const int defaultScreenSizesCount =
  sizeof( defaultScreenSizes ) / sizeof(ScreenSize);

GameGear::GameGear() : 
  Emulator( "gg", "Game Gear" ),
  m_configManager( *this ),
  m_dbManager( *this ),
  m_menuManager( *this )
{
  // The emulator screen size
  m_emulatorScreenSize.w = 256;
  m_emulatorScreenSize.h = 480;

  // Set user screen sizes
  int defaultIndex = 3;
  m_screenSize.w = defaultScreenSizes[defaultIndex].r.w; 
  m_screenSize.h = defaultScreenSizes[defaultIndex].r.h; 

  m_doubleStrike = DOUBLE_STRIKE_DISABLED;
}

ConfigManager& GameGear::getConfigManager()
{
  return m_configManager;
}

DatabaseManager& GameGear::getDbManager()
{
  return m_dbManager;
}

MenuManager& GameGear::getMenuManager()
{
  return m_menuManager;
}

extern bool DNeedRewind;
static bool specialheld = false;

void GameGear::updateControls( bool isRapid )
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
      u32 val = GameGearDbManager::GG_BUTTONS[ i ].button;
      if( val == GG_REWIND )
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
    result|=GG_RIGHT;
  IF_LEFT
    result|=GG_LEFT;
  IF_UP
    result|=GG_UP;
  IF_DOWN
    result|=GG_DOWN;

  m_padData[0] = result;

  if( !special )
  {
    specialheld = false;
    DNeedRewind = false;
  }
}

void GameGear::onPostLoad()
{
  specialheld = false;
  DNeedRewind = false;
}

bool GameGear::updateDebugText( 
  char* output, const char* defaultOutput, int len )
{
  return false;
}

bool GameGear::isRotationSupported()
{
  return false;
}

const ScreenSize* GameGear::getDefaultScreenSizes()
{
  return defaultScreenSizes;
}

int GameGear::getDefaultScreenSizesCount()
{
  return defaultScreenSizesCount;
}

const ScreenSize* GameGear::getDoubleStrikeScreenSize()
{
  return &defaultScreenSizes[1];
}

bool GameGear::isRewindSupported()
{
  return true;
}
