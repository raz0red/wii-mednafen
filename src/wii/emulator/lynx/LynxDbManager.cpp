#include "main.h"
#include "Lynx.h"
#include "LynxDbManager.h"
#include <wiiuse/wpad.h>

#define LYNX_MAP_NONE    KEY_MAP_NONE
#define LYNX_MAP_A       1
#define LYNX_MAP_B       2
#define LYNX_MAP_OPT1    3
#define LYNX_MAP_OPT2    4
#define LYNX_MAP_PAUSE   5
#define LYNX_MAP_LEFT    6
#define LYNX_MAP_RIGHT   7
#define LYNX_MAP_UP      8
#define LYNX_MAP_DOWN    9

const MappableButton LynxDbManager::LYNX_BUTTONS[LYNX_BUTTON_COUNT] = 
{
  { "(none)",   LYNX_NONE  },
  { "A",        LYNX_A     }, 
  { "B",        LYNX_B     }, 
  { "Option 1", LYNX_OPT1  },
  { "Option 2", LYNX_OPT2  },
  { "Pause",    LYNX_PAUSE },
  { "Left",     LYNX_LEFT  },
  { "Right",    LYNX_RIGHT },
  { "Up",       LYNX_UP    },
  { "Down",     LYNX_DOWN  }
};

const WiiButton LynxDbManager::
  WII_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT] =
{
  {
    { "Plus",   WPAD_BUTTON_PLUS,   LYNX_MAP_OPT1  },
    { "Minus",  WPAD_BUTTON_MINUS,  LYNX_MAP_OPT2  },
    { "2",      WPAD_BUTTON_2,      LYNX_MAP_A     }, 
    { "1",      WPAD_BUTTON_1,      LYNX_MAP_B     },
    { "A",      WPAD_BUTTON_A,      LYNX_MAP_PAUSE },
    { "B",      WPAD_BUTTON_B,      LYNX_MAP_NONE  },
    { NULL,     0,                  LYNX_MAP_NONE  },
    { NULL,     0,                  LYNX_MAP_NONE  },
    { NULL,     0,                  LYNX_MAP_NONE  },
    { NULL,     0,                  LYNX_MAP_NONE  }
  },
  {
    { "Plus",   WPAD_BUTTON_PLUS,       LYNX_MAP_OPT1  },
    { "Minus",  WPAD_BUTTON_MINUS,      LYNX_MAP_OPT2  },
    { "2",      WPAD_BUTTON_2,          LYNX_MAP_PAUSE }, 
    { "1",      WPAD_BUTTON_1,          LYNX_MAP_PAUSE },
    { "A",      WPAD_BUTTON_A,          LYNX_MAP_A     },
    { "B",      WPAD_BUTTON_B,          LYNX_MAP_NONE  },
    { "C",      WPAD_NUNCHUK_BUTTON_C,  LYNX_MAP_B     },
    { "Z",      WPAD_NUNCHUK_BUTTON_Z,  LYNX_MAP_NONE  },
    { NULL,     0,                      LYNX_MAP_NONE  },
    { NULL,     0,                      LYNX_MAP_NONE  }
  },
  {
    { "Plus",   WPAD_CLASSIC_BUTTON_PLUS,     LYNX_MAP_OPT1  },
    { "Minus",  WPAD_CLASSIC_BUTTON_MINUS,    LYNX_MAP_OPT2  },
    { "A",      WPAD_CLASSIC_BUTTON_A,        LYNX_MAP_A     }, 
    { "B",      WPAD_CLASSIC_BUTTON_B,        LYNX_MAP_B     },
    { "X",      WPAD_CLASSIC_BUTTON_X,        LYNX_MAP_NONE  },
    { "Y",      WPAD_CLASSIC_BUTTON_Y,        LYNX_MAP_NONE  },
    { "R",      WPAD_CLASSIC_BUTTON_FULL_R,   LYNX_MAP_PAUSE },
    { "L",      WPAD_CLASSIC_BUTTON_FULL_L,   LYNX_MAP_PAUSE },
    { "zR",     WPAD_CLASSIC_BUTTON_ZR,       LYNX_MAP_NONE  },
    { "zL",     WPAD_CLASSIC_BUTTON_ZL,       LYNX_MAP_NONE  }
  },
  {
    { "Start",  PAD_BUTTON_START,             LYNX_MAP_PAUSE },
    { "A",      PAD_BUTTON_A,                 LYNX_MAP_A     },
    { "B",      PAD_BUTTON_B,                 LYNX_MAP_B     }, 
    { "X",      PAD_BUTTON_X,                 LYNX_MAP_NONE  },
    { "Y",      PAD_BUTTON_Y,                 LYNX_MAP_NONE  },
    { "R",      PAD_TRIGGER_R,                LYNX_MAP_OPT1  },
    { "L",      PAD_TRIGGER_L,                LYNX_MAP_OPT2  },
    { NULL,     0,                            LYNX_MAP_NONE  },
    { NULL,     0,                            LYNX_MAP_NONE  },
    { NULL,     0,                            LYNX_MAP_NONE  },
  }
};

LynxDbManager::LynxDbManager( Emulator &emulator ) :
  StandardDatabaseManager( emulator )
{
}

dbEntry* LynxDbManager::getEntry()
{
  return (dbEntry*)&m_entry;
}

void LynxDbManager::resetToDefaults()
{
  Lynx& emu = (Lynx&)getEmulator();
  LynxDbEntry* entry = (LynxDbEntry*)getEntry();
  memset( entry, 0x0, sizeof( LynxDbEntry ) );

  StandardDatabaseManager::resetToDefaults();

  entry->orient = MDFNGameInfo->rotated;
  resetButtons();
  applyButtonMap();
}

int LynxDbManager::getMappableButtonCount()
{
  return LYNX_BUTTON_COUNT;
}

const MappableButton* LynxDbManager::getMappableButton( int button )
{
  return &LYNX_BUTTONS[button];
}

const WiiButton* LynxDbManager::getMappedButton(
  int profile, int controller, int button )
{
  return &(WII_BUTTONS[controller][button]);
}

bool LynxDbManager::writeEntryValues( 
  FILE* file, const char* hash, const dbEntry *entry )
{
  if( !StandardDatabaseManager::writeEntryValues( file, hash, entry ) )
  {
    return false;
  }

  LynxDbEntry* lynxEntry = (LynxDbEntry*)entry;
  fprintf( file, "orient=%d\n", lynxEntry->orient );

  return true;
}

void LynxDbManager::readEntryValue( 
  dbEntry *entry, const char* name, const char* value )
{
  StandardDatabaseManager::readEntryValue( entry, name, value );

  LynxDbEntry* lynxEntry = (LynxDbEntry*)entry;
  if( !strcmp( name, "orient" ) )
  {
    lynxEntry->orient = Util_sscandec( value );
  }          
}