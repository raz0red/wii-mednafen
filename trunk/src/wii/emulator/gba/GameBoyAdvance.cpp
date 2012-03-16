#include "main.h"

#include "ControlMacros.h"
#include "GameBoyAdvance.h"
#include "wii_util.h"
#include "wii_input.h"
#include "wii_hw_buttons.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"

bool wii_external_gba_bios;

static const ScreenSize defaultScreenSizes[] = 
{
  { { 320,        240       },  "1x"           },
  { { 320*2,      240*2     },  "2x"           },
  { { 320*2.5,    240*2.5   },  "2.5x"         }, 
  { { 320*2.666,  240*2.666 },  "Full screen"  }, // default
  { { 320*2.666,  240*2.825 },  "Fill screen"  }
};

static const int defaultScreenSizesCount =
  sizeof( defaultScreenSizes ) / sizeof(ScreenSize);

GameBoyAdvance::GameBoyAdvance() : 
  Emulator( "gba", "GameBoy Advance" ),
  m_configManager( *this ),
  m_dbManager( *this ),
  m_menuManager( *this ),
  m_gbaBios( false )
{
  wii_external_gba_bios = m_gbaBios;

  // The emulator screen size
  m_emulatorScreenSize.w = 240;
  m_emulatorScreenSize.h = 160;

  // Set user screen sizes
  int defaultIndex = 3;
  m_screenSize.w = defaultScreenSizes[defaultIndex].r.w; 
  m_screenSize.h = defaultScreenSizes[defaultIndex].r.h; 

  m_doubleStrike = DOUBLE_STRIKE_DISABLED;
}

ConfigManager& GameBoyAdvance::getConfigManager()
{
  return m_configManager;
}

DatabaseManager& GameBoyAdvance::getDbManager()
{
  return m_dbManager;
}

MenuManager& GameBoyAdvance::getMenuManager()
{
  return m_menuManager;
}

extern bool DNeedRewind;
static bool specialheld = false;

void GameBoyAdvance::updateControls( bool isRapid )
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
      u32 val = GameBoyAdvanceDbManager::GBA_BUTTONS[ i ].button;
      if( val == GBA_REWIND )
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
    result|=GBA_RIGHT;
  IF_LEFT
    result|=GBA_LEFT;
  IF_UP
    result|=GBA_UP;
  IF_DOWN
    result|=GBA_DOWN;

  m_padData[0] = result;

  if( !special )
  {
    specialheld = false;
    DNeedRewind = false;
  }
}

void GameBoyAdvance::onPostLoad()
{
  specialheld = false;
  DNeedRewind = false;
}

bool GameBoyAdvance::updateDebugText( 
  char* output, const char* defaultOutput, int len )
{
  return false;
}

bool GameBoyAdvance::isRotationSupported()
{
  return false;
}

u8 GameBoyAdvance::getBpp()
{
  return GBA_BPP;
}

void GameBoyAdvance::setGbaBiosEnabled( bool enabled )
{
  wii_external_gba_bios = m_gbaBios = enabled;
}

bool GameBoyAdvance::isGbaBiosEnabled()
{
  return m_gbaBios;
}

const ScreenSize* GameBoyAdvance::getDefaultScreenSizes()
{
  return defaultScreenSizes;
}

int GameBoyAdvance::getDefaultScreenSizesCount()
{
  return defaultScreenSizesCount;
}

const ScreenSize* GameBoyAdvance::getDoubleStrikeScreenSize()
{
  return &defaultScreenSizes[1];
}

bool GameBoyAdvance::isRewindSupported()
{
#ifdef ENABLE_GBA_REWIND  
  return true;
#else
  return false;
#endif
}
