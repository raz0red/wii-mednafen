#include "main.h"

#include "ControlMacros.h"
#include "Nes.h"
#include "wii_util.h"
#include "wii_input.h"
#include "wii_hw_buttons.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"  
#endif

static const ScreenSize defaultScreenSizes[] = 
{
  { { 320*1*1.25, 240    },  "1x"               },
  { { 320*2*1.25, 240*2  },  "2x/Full screen"   }
};

static const int defaultScreenSizesCount =
  sizeof( defaultScreenSizes ) / sizeof(ScreenSize);

Nes::Nes() : 
  Emulator( "nes", "NES" ),
  m_configManager( *this ),
  m_dbManager( *this ),
  m_menuManager( *this ),
  m_gameGenie( false )
{
  // The emulator screen size
  m_emulatorScreenSize.w = 256;
  m_emulatorScreenSize.h = 256;

  // Set user screen sizes
  int defaultIndex = 1;
  m_screenSize.w = defaultScreenSizes[defaultIndex].r.w; 
  m_screenSize.h = defaultScreenSizes[defaultIndex].r.h; 
}

ConfigManager& Nes::getConfigManager()
{
  return m_configManager;
}

DatabaseManager& Nes::getDbManager()
{
  return m_dbManager;
}

MenuManager& Nes::getMenuManager()
{
  return m_menuManager;
}

extern bool NESIsVSUni;
extern void MDFN_VSUniCoin();
extern MDFNGI *MDFNGameInfo;
extern int FDS_DiskInsert(int oride);
extern int FDS_DiskEject(void);
extern int FDS_DiskSelect(void);
extern bool DNeedRewind;

static int flipdisk = 0;
static bool specialheld = false;

void Nes::updateControls( bool isRapid )
{
  WPAD_ScanPads();
  PAD_ScanPads();

  if( flipdisk )
  {
    switch( flipdisk )
    {
      case 30:
        FDS_DiskEject();
        break;
      case 20:
        FDS_DiskSelect();
        break;
      case 10:
        FDS_DiskInsert(-1);
        break;
    }
    flipdisk--;
  }

  bool special = false;
  for( int c = 0; c < 4; c++ )
  {
    READ_CONTROLS_STATE

    u16 result = 0;
    StandardDbEntry* entry = (StandardDbEntry*)getDbManager().getEntry();
    StandardDatabaseManager& dbManager = 
      (StandardDatabaseManager&)getDbManager();

    for( int i = 0; i < dbManager.getMappableButtonCount(); i++ )
    {
      BEGIN_IF_BUTTON_HELD
        u32 val = NesDbManager::NES_BUTTONS[ i ].button;
        if( val == NES_REWIND )
        {
          special = true;
          if( !specialheld )
          {
            specialheld = true;
            DNeedRewind = true;
          }                    
        }
        else if( val == NES_SPECIAL )
        {          
          special = true;
          if( !specialheld )
          {
            specialheld = true;
            if( NESIsVSUni )
            {
              MDFN_VSUniCoin();
            }
            else if( MDFNGameInfo->GameType == GMT_DISK && !flipdisk )
            {
              flipdisk = 30;
            }
          }                    
        }
        else 
        {
          if( !( val & BTN_RAPID ) || isRapid )
          {
            result |= ( val & 0xFFFF );          
          }
        }
      END_IF_BUTTON_HELD
    }    

    IF_RIGHT
      result|=NES_RIGHT;
    IF_LEFT
      result|=NES_LEFT;
    IF_UP
      result|=NES_UP;
    IF_DOWN
      result|=NES_DOWN;

    m_padData[c] = result;
  }

  if( !special )
  {
    specialheld = false;
    DNeedRewind = false;
  }
}

void Nes::onPostLoad()
{
  flipdisk = 0;
  DNeedRewind = false;
  specialheld = false;
}

bool Nes::updateDebugText( 
  char* output, const char* defaultOutput, int len )
{
  return false;
}

bool Nes::isRotationSupported()
{
  return false;
}

bool Nes::isGameGenieEnabled()
{
  return m_gameGenie;
}

void Nes::setGameGenieEnabled( bool enabled )
{
  m_gameGenie = enabled;
}

u8 Nes::getBpp()
{
  return NES_BPP;
}

const ScreenSize* Nes::getDefaultScreenSizes()
{
  return defaultScreenSizes;
}

int Nes::getDefaultScreenSizesCount()
{
  return defaultScreenSizesCount;
}

const ScreenSize* Nes::getDoubleStrikeScreenSize()
{
  return &defaultScreenSizes[1];
}

bool Nes::isRewindSupported()
{
  return MDFNGameInfo->GameType != GMT_DISK;
}