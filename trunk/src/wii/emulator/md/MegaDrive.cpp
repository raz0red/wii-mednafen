#include "main.h"

#include "genio.h"

#include "ControlMacros.h"
#include "MegaDrive.h"
#include "wii_util.h"
#include "wii_input.h"
#include "wii_hw_buttons.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"

const char* MegaDrive::regions[4] = 
{ 
   "game", 
   "overseas_ntsc", 
   "overseas_pal", 
   "domestic_ntsc"
};

const char* MegaDrive::regionNames[4] = 
{ 
   "Match game header", 
   "North America", 
   "Europe", 
   "Japan"
};

const int MegaDrive::regionCount = 
  sizeof( regions ) / sizeof( const char* );

static const ScreenSize defaultScreenSizes[] = 
{
  { { 320*1*1.111, 240    },  "1x"               },
  { { 320*2*1.111, 240*2  },  "2x/Full screen"   }
};

static const int defaultScreenSizesCount =
  sizeof( defaultScreenSizes ) / sizeof(ScreenSize);

MegaDrive::MegaDrive() : 
  Emulator( "md", "Mega Drive" ),
  m_configManager( *this ),
  m_dbManager( *this ),
  m_menuManager( *this ),
  m_consoleRegion( 0 )
{
  // The emulator screen size
  m_emulatorScreenSize.w = 320;
  m_emulatorScreenSize.h = 480;

  // Set user screen sizes
  int defaultIndex = 1;
  m_screenSize.w = defaultScreenSizes[defaultIndex].r.w; 
  m_screenSize.h = defaultScreenSizes[defaultIndex].r.h; 
}

ConfigManager& MegaDrive::getConfigManager()
{
  return m_configManager;
}

DatabaseManager& MegaDrive::getDbManager()
{
  return m_dbManager;
}

MenuManager& MegaDrive::getMenuManager()
{
  return m_menuManager;
}

extern bool DNeedRewind;
static bool specialheld = false;

void MegaDrive::updateControls( bool isRapid )
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
        u32 val = MegaDriveDbManager::MD_BUTTONS[ i ].button;
        if( val == MD_REWIND )
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
      result|=MD_RIGHT;
    IF_LEFT
      result|=MD_LEFT;
    IF_UP
      result|=MD_UP;
    IF_DOWN
      result|=MD_DOWN;

    m_padData[c] = result;
  }

  if( !special )
  {
    specialheld = false;
    DNeedRewind = false;
  }
}

void MegaDrive::onPostLoad()
{
  specialheld = false;
  DNeedRewind = false;
}

void MegaDrive::onPreLoop()
{
  MegaDriveDbEntry* entry = (MegaDriveDbEntry*)getDbManager().getEntry();
  for( int c = 0; c < 2; c++ )
  {
    int ctype = entry->controlType[c];
    MDFN_IEN_MD::MDINPUT_SetInput(
      c, ( ctype == MD_CONTROL_3BUTTON ? "gamepad" : "gamepad6" ), NULL);
  }
}

bool MegaDrive::updateDebugText( 
  char* output, const char* defaultOutput, int len )
{
  return false;
}

bool MegaDrive::isRotationSupported()
{
  return false;
}

void MegaDrive::setConsoleRegion( int region )
{
  if( region >= 0 && region < regionCount )
  { 
    m_consoleRegion = region;
  }
}

int MegaDrive::getConsoleRegion()
{
  return m_consoleRegion;
}

const char* MegaDrive::getConsoleRegionString()
{
  return MegaDrive::regions[m_consoleRegion];
}

const char* MegaDrive::getConsoleRegionName()
{
  return MegaDrive::regionNames[m_consoleRegion];
}

const ScreenSize* MegaDrive::getDefaultScreenSizes()
{
  return defaultScreenSizes;
}

int MegaDrive::getDefaultScreenSizesCount()
{
  return defaultScreenSizesCount;
}

u8 MegaDrive::getBpp()
{
  return MD_BPP;
}

const ScreenSize* MegaDrive::getDoubleStrikeScreenSize()
{
  return &defaultScreenSizes[1];
}

bool MegaDrive::isRewindSupported()
{
#ifdef ENABLE_MD_REWIND    
  return true;
#else
  return false;
#endif
}