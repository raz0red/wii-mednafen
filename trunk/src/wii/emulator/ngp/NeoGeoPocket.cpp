#include "main.h"

#include "ControlMacros.h"
#include "NeoGeoPocket.h"
#include "wii_util.h"
#include "wii_input.h"
#include "wii_hw_buttons.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"

static const ScreenSize defaultScreenSizes[] = 
{
  { { 320,         240        },  "1x"          },
  { { 320*2,       240*2      },  "2x"          },
  { { 320*2.973,   240*2.973  },  "Full screen" },
  { { 320*4,       240*2.973  },  "Fill screen" }
};

static const int defaultScreenSizesCount =
  sizeof( defaultScreenSizes ) / sizeof(ScreenSize);

NeoGeoPocket::NeoGeoPocket() : 
  Emulator( "ngp", "Neo Geo Pocket" ),
  m_configManager( *this ),
  m_dbManager( *this ),
  m_menuManager( *this ),
  m_gameLanguage( NGP_LANG_ENGLISH )
{
  // The emulator screen size
  m_emulatorScreenSize.w = 160;
  m_emulatorScreenSize.h = 152;

  // Set user screen sizes
  int defaultIndex = 2;
  m_screenSize.w = defaultScreenSizes[defaultIndex].r.w; 
  m_screenSize.h = defaultScreenSizes[defaultIndex].r.h; 

  m_doubleStrike = DOUBLE_STRIKE_DISABLED;
}

ConfigManager& NeoGeoPocket::getConfigManager()
{ 
  return m_configManager;
}

DatabaseManager& NeoGeoPocket::getDbManager()
{
  return m_dbManager;
}
 
MenuManager& NeoGeoPocket::getMenuManager()
{
  return m_menuManager;
}

extern bool DNeedRewind;
static bool specialheld = false;

void NeoGeoPocket::updateControls( bool isRapid )
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
      u32 val = NeoGeoPocketDbManager::NGP_BUTTONS[ i ].button;
      if( val == NGP_REWIND )
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
    result|=NGP_RIGHT;
  IF_LEFT
    result|=NGP_LEFT;
  IF_UP
    result|=NGP_UP;
  IF_DOWN
    result|=NGP_DOWN;

  m_padData[0] = result;

  if( !special )
  {
    specialheld = false;
    DNeedRewind = false;
  }
}

void NeoGeoPocket::onPostLoad()
{
  specialheld = false;
  DNeedRewind = false;
}

bool NeoGeoPocket::updateDebugText( 
  char* output, const char* defaultOutput, int len )
{
  return false;
}

bool NeoGeoPocket::isRotationSupported()
{
  return false;
}

int NeoGeoPocket::getGameLanguage()
{
  return m_gameLanguage;
}

void  NeoGeoPocket::setGameLanguage( int lang )
{
  m_gameLanguage = lang;
}

const ScreenSize* NeoGeoPocket::getDefaultScreenSizes()
{
  return defaultScreenSizes;
}

int NeoGeoPocket::getDefaultScreenSizesCount()
{
  return defaultScreenSizesCount;
}

const ScreenSize* NeoGeoPocket::getDoubleStrikeScreenSize()
{
  return &defaultScreenSizes[1];
}

bool NeoGeoPocket::isRewindSupported()
{
  return true;
}