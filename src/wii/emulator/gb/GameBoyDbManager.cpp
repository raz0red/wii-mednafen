#include "main.h"
#include "GameBoy.h"
#include "GameBoyDbManager.h"
#include <wiiuse/wpad.h>

#define GB_MAP_NONE    KEY_MAP_NONE
#define GB_MAP_A       1
#define GB_MAP_B       2
#define GB_MAP_START   3
#define GB_MAP_SELECT  4
#define GB_MAP_LEFT    5
#define GB_MAP_RIGHT   6
#define GB_MAP_UP      7
#define GB_MAP_DOWN    8
#define GB_MAP_A_R     9
#define GB_MAP_B_R     10
#define GB_MAP_REWIND  11

const MappableButton GameBoyDbManager::GB_BUTTONS[] = 
{
  { "(none)",   GB_NONE    },
  { "A",        GB_A       }, 
  { "B",        GB_B       }, 
  { "Start",    GB_START   },
  { "Select",   GB_SELECT  },
  { "Left",     GB_LEFT    },
  { "Right",    GB_RIGHT   },
  { "Up",       GB_UP      },
  { "Down",     GB_DOWN    },
  { "A",        GB_A_R     }, 
  { "B",        GB_B_R     },
  { "(rewind)", GB_REWIND  }  
};

static const int buttonCount =  
  sizeof(GameBoyDbManager::GB_BUTTONS)/sizeof(MappableButton);

const WiiButton GameBoyDbManager::
  WII_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT] =
{
  {
    { "Plus",   WPAD_BUTTON_PLUS,   GB_MAP_START  },
    { "Minus",  WPAD_BUTTON_MINUS,  GB_MAP_SELECT },
    { "2",      WPAD_BUTTON_2,      GB_MAP_A      }, 
    { "1",      WPAD_BUTTON_1,      GB_MAP_B      },
    { "A",      WPAD_BUTTON_A,      GB_MAP_NONE   },
    { "B",      WPAD_BUTTON_B,      GB_MAP_NONE   },
    { NULL,     0,                  GB_MAP_NONE   },
    { NULL,     0,                  GB_MAP_NONE   },
    { NULL,     0,                  GB_MAP_NONE   },
    { NULL,     0,                  GB_MAP_NONE   }
  },
  {
    { "Plus",   WPAD_BUTTON_PLUS,       GB_MAP_START  },
    { "Minus",  WPAD_BUTTON_MINUS,      GB_MAP_SELECT },
    { "2",      WPAD_BUTTON_2,          GB_MAP_NONE   }, 
    { "1",      WPAD_BUTTON_1,          GB_MAP_NONE   },
    { "A",      WPAD_BUTTON_A,          GB_MAP_A      },
    { "B",      WPAD_BUTTON_B,          GB_MAP_NONE   },
    { "C",      WPAD_NUNCHUK_BUTTON_C,  GB_MAP_B      },
    { "Z",      WPAD_NUNCHUK_BUTTON_Z,  GB_MAP_NONE   },
    { NULL,     0,                      GB_MAP_NONE   },
    { NULL,     0,                      GB_MAP_NONE   }
  },
  {
    { "Plus",   WPAD_CLASSIC_BUTTON_PLUS,     GB_MAP_START  },
    { "Minus",  WPAD_CLASSIC_BUTTON_MINUS,    GB_MAP_SELECT },
    { "A",      WPAD_CLASSIC_BUTTON_A,        GB_MAP_A      }, 
    { "B",      WPAD_CLASSIC_BUTTON_B,        GB_MAP_B      },
    { "X",      WPAD_CLASSIC_BUTTON_X,        GB_MAP_NONE   },
    { "Y",      WPAD_CLASSIC_BUTTON_Y,        GB_MAP_NONE   },
    { "R",      WPAD_CLASSIC_BUTTON_FULL_R,   GB_MAP_NONE   },
    { "L",      WPAD_CLASSIC_BUTTON_FULL_L,   GB_MAP_NONE   },
    { "zR",     WPAD_CLASSIC_BUTTON_ZR,       GB_MAP_NONE   },
    { "zL",     WPAD_CLASSIC_BUTTON_ZL,       GB_MAP_NONE   }
  },
  {
    { "Start",  PAD_BUTTON_START,             GB_MAP_START  },
    { "A",      PAD_BUTTON_A,                 GB_MAP_A      },
    { "B",      PAD_BUTTON_B,                 GB_MAP_B      }, 
    { "X",      PAD_BUTTON_X,                 GB_MAP_SELECT },
    { "Y",      PAD_BUTTON_Y,                 GB_MAP_SELECT },
    { "R",      PAD_TRIGGER_R,                GB_MAP_NONE   },
    { "L",      PAD_TRIGGER_L,                GB_MAP_NONE   },
    { NULL,     0,                            GB_MAP_NONE   },
    { NULL,     0,                            GB_MAP_NONE   },
    { NULL,     0,                            GB_MAP_NONE   },
  }
};

GameBoyDbManager::GameBoyDbManager( Emulator &emulator ) :
  StandardDatabaseManager( emulator )
{
}

dbEntry* GameBoyDbManager::getEntry()
{
  return (dbEntry*)&m_entry;
}

void GameBoyDbManager::resetToDefaults()
{
  GameBoy& emu = (GameBoy&)getEmulator();
  GameBoyDbEntry* entry = (GameBoyDbEntry*)getEntry();
  memset( entry, 0x0, sizeof( GameBoyDbEntry ) );

  StandardDatabaseManager::resetToDefaults();

  resetButtons();
  applyButtonMap();
}

int GameBoyDbManager::getMappableButtonCount( int profile )
{
  return buttonCount;
}

const MappableButton* GameBoyDbManager::getMappableButton( int profile, int button )
{
  return &GB_BUTTONS[button];
}

const WiiButton* GameBoyDbManager::getMappedButton(
  int profile, int controller, int button )
{
  return &(WII_BUTTONS[controller][button]);
}
