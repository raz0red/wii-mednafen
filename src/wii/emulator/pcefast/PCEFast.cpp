#include "main.h"

#include "ControlMacros.h"
#include "PCEFast.h"
#include "wii_util.h"
#include "wii_input.h"
#include "wii_hw_buttons.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"

static const ScreenSize defaultScreenSizes[] = 
{
  { { 320,    240        },  "1x"             },
  { { 320*2,  240*2      },  "2x/Full screen" },
};

static const int defaultScreenSizesCount =
  sizeof( defaultScreenSizes ) / sizeof(ScreenSize);

PCEFast::PCEFast() : 
  Emulator( "pce_fast", "PC Engine" ),
  m_configManager( *this ),
  m_dbManager( *this ),
  m_menuManager( *this )
{
  // The emulator screen size
  m_emulatorScreenSize.w = 512;
  m_emulatorScreenSize.h = 242;

  // Set user screen sizes
  int defaultIndex = 1;
  m_screenSize.w = defaultScreenSizes[defaultIndex].r.w; 
  m_screenSize.h = defaultScreenSizes[defaultIndex].r.h; 
}

ConfigManager& PCEFast::getConfigManager()
{
  return m_configManager;
}

DatabaseManager& PCEFast::getDbManager()
{
  return m_dbManager;
}

MenuManager& PCEFast::getMenuManager()
{
  return m_menuManager;
}

// Mednafen extern
namespace PCE_Fast
{
  extern bool AVPad6Enabled[5];
}

extern bool DNeedRewind;
static bool specialheld = false;

void PCEFast::updateControls( bool isRapid )
{
  bool special = false;

  WPAD_ScanPads();
  PAD_ScanPads();

  for( int c = 0; c < 4; c++ )
  {
    READ_CONTROLS_STATE
      
    u16 result = 0;
    StandardDbEntry* entry = (StandardDbEntry*)getDbManager().getEntry();

    // 2 or 6 button controls
    PCE_Fast::AVPad6Enabled[c] = ((PCEFastDbEntry*)entry)->controlType[c];

    StandardDatabaseManager& dbManager = 
      (StandardDatabaseManager&)getDbManager();

    for( int i = 0; i < dbManager.getMappableButtonCount( entry->profile ); i++ )
    {
      BEGIN_IF_BUTTON_HELD(entry->profile)
        u32 val = PCEFastDbManager::PCE_BUTTONS[ i ].button;
        if( val == PCE_REWIND )
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
      result|=PCE_RIGHT;
    IF_LEFT
      result|=PCE_LEFT;
    IF_UP
      result|=PCE_UP;
    IF_DOWN
      result|=PCE_DOWN;

    m_padData[c] = result;
  }

  if( !special )
  {
    specialheld = false;
    DNeedRewind = false;
  }
}

void PCEFast::onPostLoad()
{
  specialheld = false;
  DNeedRewind = false;
}

bool PCEFast::updateDebugText( 
  char* output, const char* defaultOutput, int len )
{
  return false;
}

bool PCEFast::isRotationSupported()
{
  return false;
}

u8 PCEFast::getBpp()
{
  return PCE_FAST_BPP;
}

const ScreenSize* PCEFast::getDefaultScreenSizes()
{ 
  return defaultScreenSizes;
}

int PCEFast::getDefaultScreenSizesCount()
{
  return defaultScreenSizesCount;
}

const ScreenSize* PCEFast::getDoubleStrikeScreenSize()
{
  return &defaultScreenSizes[1];
}

namespace PCE_Fast
{
  extern bool IsSGX;
  extern bool PCE_IsCD;
}

bool PCEFast::isRewindSupported()
{
  return !PCE_Fast::PCE_IsCD && !PCE_Fast::IsSGX;
}