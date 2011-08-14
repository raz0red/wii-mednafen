#include "main.h"
#include "Nes.h"
#include "NesDbManager.h"
#include <wiiuse/wpad.h>

#define NES_MAP_NONE    KEY_MAP_NONE
#define NES_MAP_A       1
#define NES_MAP_B       2
#define NES_MAP_START   3
#define NES_MAP_SELECT  4
#define NES_MAP_LEFT    5
#define NES_MAP_RIGHT   6
#define NES_MAP_UP      7
#define NES_MAP_DOWN    8
#define NES_MAP_A_R     9
#define NES_MAP_B_R     10
#define NES_MAP_SPECIAL 11

const MappableButton NesDbManager::NES_BUTTONS[NES_BUTTON_COUNT] = 
{
  { "(none)",         NES_NONE    },
  { "A",              NES_A       }, 
  { "B",              NES_B       }, 
  { "Start",          NES_START   },
  { "Select",         NES_SELECT  },
  { "Left",           NES_LEFT    },
  { "Right",          NES_RIGHT   },
  { "Up",             NES_UP      },
  { "Down",           NES_DOWN    },
  { "A",              NES_A_R     }, 
  { "B",              NES_B_R     },
  { "Coin/Flip disk", NES_SPECIAL }
};

const WiiButton NesDbManager::
  WII_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT] =
{
  {
    { "Plus",   WPAD_BUTTON_PLUS,   NES_MAP_START   },
    { "Minus",  WPAD_BUTTON_MINUS,  NES_MAP_SELECT  },
    { "2",      WPAD_BUTTON_2,      NES_MAP_A       }, 
    { "1",      WPAD_BUTTON_1,      NES_MAP_B       },
    { "A",      WPAD_BUTTON_A,      NES_MAP_SPECIAL },
    { "B",      WPAD_BUTTON_B,      NES_MAP_NONE    },
    { NULL,     0,                  NES_MAP_NONE    },
    { NULL,     0,                  NES_MAP_NONE    },
    { NULL,     0,                  NES_MAP_NONE    },
    { NULL,     0,                  NES_MAP_NONE    }
  },
  {
    { "Plus",   WPAD_BUTTON_PLUS,       NES_MAP_START   },
    { "Minus",  WPAD_BUTTON_MINUS,      NES_MAP_SELECT  },
    { "2",      WPAD_BUTTON_2,          NES_MAP_SPECIAL }, 
    { "1",      WPAD_BUTTON_1,          NES_MAP_SPECIAL },
    { "A",      WPAD_BUTTON_A,          NES_MAP_A       },
    { "B",      WPAD_BUTTON_B,          NES_MAP_NONE    },
    { "C",      WPAD_NUNCHUK_BUTTON_C,  NES_MAP_B       },
    { "Z",      WPAD_NUNCHUK_BUTTON_Z,  NES_MAP_NONE    },
    { NULL,     0,                      NES_MAP_NONE    },
    { NULL,     0,                      NES_MAP_NONE    }
  },
  {
    { "Plus",   WPAD_CLASSIC_BUTTON_PLUS,     NES_MAP_START   },
    { "Minus",  WPAD_CLASSIC_BUTTON_MINUS,    NES_MAP_SELECT  },
    { "A",      WPAD_CLASSIC_BUTTON_A,        NES_MAP_A       }, 
    { "B",      WPAD_CLASSIC_BUTTON_B,        NES_MAP_B       },
    { "X",      WPAD_CLASSIC_BUTTON_X,        NES_MAP_NONE    },
    { "Y",      WPAD_CLASSIC_BUTTON_Y,        NES_MAP_NONE    },
    { "R",      WPAD_CLASSIC_BUTTON_FULL_R,   NES_MAP_SPECIAL },
    { "L",      WPAD_CLASSIC_BUTTON_FULL_L,   NES_MAP_SPECIAL },
    { "zR",     WPAD_CLASSIC_BUTTON_ZR,       NES_MAP_SPECIAL },
    { "zL",     WPAD_CLASSIC_BUTTON_ZL,       NES_MAP_SPECIAL }
  },
  {
    { "Start",  PAD_BUTTON_START,             NES_MAP_START   },
    { "A",      PAD_BUTTON_A,                 NES_MAP_A       },
    { "B",      PAD_BUTTON_B,                 NES_MAP_B       }, 
    { "X",      PAD_BUTTON_X,                 NES_MAP_SELECT  },
    { "Y",      PAD_BUTTON_Y,                 NES_MAP_SELECT  },
    { "R",      PAD_TRIGGER_R,                NES_MAP_SPECIAL },
    { "L",      PAD_TRIGGER_L,                NES_MAP_SPECIAL },
    { NULL,     0,                            NES_MAP_NONE    },
    { NULL,     0,                            NES_MAP_NONE    },
    { NULL,     0,                            NES_MAP_NONE    }
  }
};

NesDbManager::NesDbManager( Emulator &emulator ) :
  StandardDatabaseManager( emulator )
{
}

dbEntry* NesDbManager::getEntry()
{
  return (dbEntry*)&m_entry;
}

void NesDbManager::resetToDefaults()
{
  Nes& emu = (Nes&)getEmulator();
  NesDbEntry* entry = (NesDbEntry*)getEntry();
  memset( entry, 0x0, sizeof( NesDbEntry ) );

  StandardDatabaseManager::resetToDefaults();

  resetButtons();
  applyButtonMap();
}

int NesDbManager::getMappableButtonCount()
{
  return NES_BUTTON_COUNT;
}

const MappableButton* NesDbManager::getMappableButton( int button )
{
  return &NES_BUTTONS[button];
}

const WiiButton* NesDbManager::getMappedButton(
  int profile, int controller, int button )
{
  return &(WII_BUTTONS[controller][button]);
}
