#include "main.h"

#include "ControlMacros.h"
#include "PCFX.h"
#include "wii_util.h"
#include "wii_input.h"
#include "wii_hw_buttons.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"

static const ScreenSize defaultScreenSizes[] = 
{
  { { 320*1*1.1, 240    },  "1x"               },
  { { 320*2*1.1, 240*2  },  "2x/Full screen"   }
};

static const int defaultScreenSizesCount =
  sizeof( defaultScreenSizes ) / sizeof(ScreenSize);

PCFX::PCFX() : 
  Emulator( "pcfx", "PC-FX" ),
  m_configManager( *this ),
  m_dbManager( *this ),
  m_menuManager( *this )
{
  // The emulator screen size
  m_emulatorScreenSize.w = 341;
  m_emulatorScreenSize.h = 280;

  // Set user screen sizes
  int defaultIndex = 1;
  m_screenSize.w = defaultScreenSizes[defaultIndex].r.w; 
  m_screenSize.h = defaultScreenSizes[defaultIndex].r.h; 
}

ConfigManager& PCFX::getConfigManager()
{
  return m_configManager;
}

DatabaseManager& PCFX::getDbManager()
{
  return m_dbManager;
}

MenuManager& PCFX::getMenuManager()
{
  return m_menuManager;
}

void PCFX::updateControls( bool isRapid )
{
  WPAD_ScanPads();
  PAD_ScanPads();

  for( int c = 0; c < 4; c++ )
  {
    READ_CONTROLS_STATE

    u16 result = 0;
    StandardDbEntry* entry = (StandardDbEntry*)getDbManager().getEntry();

    for( int i = 0; i < PCFX_BUTTON_COUNT; i++ )
    {
      BEGIN_IF_BUTTON_HELD(entry->profile)
        result |= PCFXDbManager::PCFX_BUTTONS[ i ].button;
      END_IF_BUTTON_HELD
    }    

    IF_RIGHT
      result|=PCFX_RIGHT;
    IF_LEFT
      result|=PCFX_LEFT;
    IF_UP
      result|=PCFX_UP;
    IF_DOWN
      result|=PCFX_DOWN;

    m_padData[c] = result;
  }
}

void PCFX::onPostLoad()
{
}

bool PCFX::updateDebugText( 
  char* output, const char* defaultOutput, int len )
{
  return false;
}

bool PCFX::isRotationSupported()
{
  return false;
}

u8 PCFX::getBpp()
{
  return PCFX_BPP;
}

const ScreenSize* PCFX::getDefaultScreenSizes()
{
  return defaultScreenSizes;
}

int PCFX::getDefaultScreenSizesCount()
{
  return defaultScreenSizesCount;
}

const ScreenSize* PCFX::getDoubleStrikeScreenSize()
{
  return &defaultScreenSizes[1];
}