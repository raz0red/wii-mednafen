#include "main.h"

#include "ControlMacros.h"
#include "Lynx.h"
#include "wii_util.h"
#include "wii_input.h"
#include "wii_hw_buttons.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"

static const ScreenSize defaultScreenSizes[] = 
{
  { { 320,        240       },  "1x"              },
  { { 320*2,      240*2     },  "2x"              },
  { { 320*3,      240*3     },  "3x"              }, 
  { { 320*4,      240*4     },  "4x/Full screen"  }, // default
  { { 320*4,      240*4.431 },  "Fill screen"     }
};

static const int defaultScreenSizesCount =
  sizeof( defaultScreenSizes ) / sizeof(ScreenSize);

static const ScreenSize defaultRotatedScreenSizes[] = 
{
  { { 320,        240       },   "1x"              },
  { { 320*2,      240*2     },   "2x"              },
  { { 320*2.825,  240*2.825 },   "3x/Full screen"  }, // default 
  { { 320*2.825,  240*6.27  },   "Fill screen"     }
};

static const int defaultRotatedScreenSizesCount =
  sizeof( defaultRotatedScreenSizes ) / sizeof(ScreenSize);

Lynx::Lynx() : 
  Emulator( "lynx", "Lynx" ),
  m_configManager( *this ),
  m_dbManager( *this ),
  m_menuManager( *this )
{
  // The emulator screen size
  m_emulatorScreenSize.w = 160;
  m_emulatorScreenSize.h = 102;

  // Set user screen sizes
  int defaultIndex = 3;
  m_screenSize.w = defaultScreenSizes[defaultIndex].r.w; 
  m_screenSize.h = defaultScreenSizes[defaultIndex].r.h; 

  defaultIndex = 2;
  m_rotatedScreenSize.w = 
    defaultRotatedScreenSizes[defaultIndex].r.w; 
  m_rotatedScreenSize.h = 
    defaultRotatedScreenSizes[defaultIndex].r.h; 

  m_doubleStrike = DOUBLE_STRIKE_DISABLED;
}

ConfigManager& Lynx::getConfigManager()
{
  return m_configManager;
}

DatabaseManager& Lynx::getDbManager()
{
  return m_dbManager;
}

MenuManager& Lynx::getMenuManager()
{
  return m_menuManager;
}

extern bool DNeedRewind;
static bool specialheld = false;

void Lynx::updateControls( bool isRapid )
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
      u32 val = LynxDbManager::LYNX_BUTTONS[ i ].button;
      if( val == LYNX_REWIND )
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

  int left, right, up, down;
  switch( getRotation() )
  {
    case MDFN_ROTATE0:
      up = LYNX_UP;
      left = LYNX_LEFT;
      down = LYNX_DOWN;
      right = LYNX_RIGHT;
      break;
    case MDFN_ROTATE270:
      up = LYNX_LEFT;
      left = LYNX_DOWN;
      down = LYNX_RIGHT;
      right = LYNX_UP;
      break;
    case MDFN_ROTATE90:
      up = LYNX_RIGHT;
      left = LYNX_UP;
      down = LYNX_LEFT;
      right = LYNX_DOWN;
  }

  IF_RIGHT
    result|=right;
  IF_LEFT
    result|=left;
  IF_UP
    result|=up;
  IF_DOWN
    result|=down;

  m_padData[0] = result;

  if( !special )
  {
    specialheld = false;
    DNeedRewind = false;
  }
}

void Lynx::onPostLoad()
{
  specialheld = false;
  DNeedRewind = false;
}

bool Lynx::updateDebugText( 
  char* output, const char* defaultOutput, int len )
{
  return false;
}

bool Lynx::isRotationSupported()
{
  return true;
}

int Lynx::getRotation()
{
  return 
    ((LynxDbEntry*)m_dbManager.getEntry())->orient;
}

const ScreenSize* Lynx::getDefaultScreenSizes()
{
  return defaultScreenSizes;
}

int Lynx::getDefaultScreenSizesCount()
{
  return defaultScreenSizesCount;
}

const ScreenSize* Lynx::getDefaultRotatedScreenSizes()
{
  return defaultRotatedScreenSizes;
}

int Lynx::getDefaultRotatedScreenSizesCount()
{
  return defaultRotatedScreenSizesCount;
}

const ScreenSize* Lynx::getDoubleStrikeScreenSize()
{
  return &defaultScreenSizes[1];
}

const ScreenSize* Lynx::getDoubleStrikeRotatedScreenSize()
{
  return &defaultRotatedScreenSizes[1];
}

bool Lynx::isRewindSupported()
{
  return true;
}