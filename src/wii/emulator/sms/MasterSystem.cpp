#include "main.h"

#include "ControlMacros.h"
#include "MasterSystem.h"
#include "wii_util.h"
#include "wii_input.h"
#include "wii_hw_buttons.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"

const char* MasterSystem::regions[2] = 
{ 
   "domestic", 
   "export", 
};

const char* MasterSystem::regionNames[2] = 
{ 
   "Domestic (Japan)", 
   "Export (World)"
};

static const ScreenSize defaultScreenSizes[] = 
{
  { { 320*1*1.25, 240    },  "1x"               },
  { { 320*2*1.25, 240*2  },  "2x/Full screen"   }
};

static const int defaultScreenSizesCount =
  sizeof( defaultScreenSizes ) / sizeof(ScreenSize);

const int MasterSystem::regionCount = 
  sizeof( regions ) / sizeof( const char* );


MasterSystem::MasterSystem() : 
  Emulator( "sms", "Sega Master System" ),
  m_configManager( *this ),
  m_dbManager( *this ),
  m_menuManager( *this ),
  m_consoleRegion( 1 )
{
  // The emulator screen size
  m_emulatorScreenSize.w = 256;
  m_emulatorScreenSize.h = 480;
 
  // Set user screen sizes
  int defaultIndex = 1;
  m_screenSize.w = defaultScreenSizes[defaultIndex].r.w; 
  m_screenSize.h = defaultScreenSizes[defaultIndex].r.h; 
}

ConfigManager& MasterSystem::getConfigManager()
{
  return m_configManager;
}

DatabaseManager& MasterSystem::getDbManager()
{
  return m_dbManager;
}

MenuManager& MasterSystem::getMenuManager()
{
  return m_menuManager;
}

extern bool DNeedRewind;
static bool specialheld = false;

void MasterSystem::updateControls( bool isRapid )
{
  bool special = false;

  WPAD_ScanPads();
  PAD_ScanPads();

  for( int c = 0; c < 2; c++ )
  {
    READ_CONTROLS_STATE

    u16 result = 0;
    StandardDbEntry* entry = (StandardDbEntry*)getDbManager().getEntry();

    StandardDatabaseManager& dbManager = 
      (StandardDatabaseManager&)getDbManager();

    for( int i = 0; i < dbManager.getMappableButtonCount( entry->profile ); i++ )  
    {
      BEGIN_IF_BUTTON_HELD(entry->profile)
        u32 val = MasterSystemDbManager::SMS_BUTTONS[ i ].button;
        if( val == SMS_REWIND )
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
      result|=SMS_RIGHT;
    IF_LEFT
      result|=SMS_LEFT;
    IF_UP
      result|=SMS_UP;
    IF_DOWN
      result|=SMS_DOWN;

    m_padData[c] = result;
  }

  if( !special )
  {
    specialheld = false;
    DNeedRewind = false;
  }
}

void MasterSystem::setConsoleRegion( int region )
{
  if( region >= 0 && region < regionCount )
  { 
    m_consoleRegion = region;
  }
}

int MasterSystem::getConsoleRegion()
{
  return m_consoleRegion;
}

const char* MasterSystem::getConsoleRegionString()
{
  return MasterSystem::regions[m_consoleRegion];
}

const char* MasterSystem::getConsoleRegionName()
{
  return MasterSystem::regionNames[m_consoleRegion];
}

void MasterSystem::onPostLoad()
{
  specialheld = false;
  DNeedRewind = false;
}

bool MasterSystem::updateDebugText( 
  char* output, const char* defaultOutput, int len )
{
  return false;
}

bool MasterSystem::isRotationSupported()
{
  return false;
}

const ScreenSize* MasterSystem::getDefaultScreenSizes()
{
  return defaultScreenSizes;
}

int MasterSystem::getDefaultScreenSizesCount()
{
  return defaultScreenSizesCount;
}

const ScreenSize* MasterSystem::getDoubleStrikeScreenSize()
{
  return &defaultScreenSizes[1];
}

bool MasterSystem::isRewindSupported()
{
  return true;
}