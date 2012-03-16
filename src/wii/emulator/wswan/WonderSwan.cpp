#include "main.h"

#include "ControlMacros.h"
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
  m_menuManager( *this ),
  m_gameLanguage( WS_LANG_ENGLISH )
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

extern bool DNeedRewind;
static bool specialheld = false;

void WonderSwan::updateControls( bool isRapid )
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
      u32 val = WonderSwanDbManager::WS_BUTTONS[ i ].button;
      if( val == WS_REWIND )
      {
        special = true;
        if( !specialheld )
        {
          specialheld = true;
          DNeedRewind = true;
        }                    
      }
      else if( val == WS_ROTATE )
      {        
        special = true;
        if( !specialheld )
        {
          specialheld = true;

          // Swap between the two profiles (normal and rotated)
          entry->profile ^= 1;
          WII_SetRotation( entry->profile * 90 ); // Rotate screen
          wii_mednafen_reset_last_rect(); // Clear last rect cache (reset)
        }     
      }
      else if( !( val & BTN_RAPID ) || isRapid )
      {
        result |= ( val & 0xFFFF );
      }
    END_IF_BUTTON_HELD
  }    

  bool rot = ( entry->profile == 1 );

  IF_RIGHT
    result|=( rot ? WS_Y3 : WS_X2 );
  IF_LEFT
    result|=( rot ? WS_Y1 : WS_X4 );
  IF_UP
    result|=( rot ? WS_Y2 : WS_X1 );
  IF_DOWN
    result|=( rot ? WS_Y4 : WS_X3 );

  m_padData[0] = result;

  if( !special )
  {
    specialheld = false;
    DNeedRewind = false;
  }
}

void WonderSwan::onPostLoad()
{
  specialheld = false;
  DNeedRewind = false;
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

namespace MDFN_IEN_WSWAN {
  extern int WS_SramSize;
}

bool WonderSwan::isRewindSupported()
{
  return MDFN_IEN_WSWAN::WS_SramSize <= 32*1024;
}

int WonderSwan::getGameLanguage()
{
  return m_gameLanguage;
}

void  WonderSwan::setGameLanguage( int lang )
{
  m_gameLanguage = lang;
}
