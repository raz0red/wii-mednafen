#include "main.h"
#include "GameBoyAdvance.h"
#include "GameBoyAdvanceDbManager.h"
#include <wiiuse/wpad.h>

#define GBA_MAP_NONE    KEY_MAP_NONE
#define GBA_MAP_A       1
#define GBA_MAP_B       2
#define GBA_MAP_L       3
#define GBA_MAP_R       4
#define GBA_MAP_START   5
#define GBA_MAP_SELECT  6
#define GBA_MAP_LEFT    7
#define GBA_MAP_RIGHT   8
#define GBA_MAP_UP      9
#define GBA_MAP_DOWN    10
#define GBA_MAP_A_R     11
#define GBA_MAP_B_R     12
#define GBA_MAP_REW     13

const MappableButton GameBoyAdvanceDbManager::GBA_BUTTONS[] = 
{
  { "(none)",   GBA_NONE    },
  { "A",        GBA_A       }, 
  { "B",        GBA_B       }, 
  { "L",        GBA_L       },
  { "R",        GBA_R       },
  { "Start",    GBA_START   },
  { "Select",   GBA_SELECT  },
  { "Left",     GBA_LEFT    },
  { "Right",    GBA_RIGHT   },
  { "Up",       GBA_UP      },
  { "Down",     GBA_DOWN    },
  { "A",        GBA_A_R     }, 
  { "B",        GBA_B_R     },
#ifdef ENABLE_GBA_REWIND    
  { "(rewind)", GBA_REWIND  } 
#endif
};

static const int buttonCount =  
  sizeof(GameBoyAdvanceDbManager::GBA_BUTTONS)/sizeof(MappableButton);

const WiiButton GameBoyAdvanceDbManager::
  WII_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT] =
{
  {
    { "Plus",   WPAD_BUTTON_PLUS,   GBA_MAP_START  },
    { "Minus",  WPAD_BUTTON_MINUS,  GBA_MAP_SELECT },
    { "2",      WPAD_BUTTON_2,      GBA_MAP_A      }, 
    { "1",      WPAD_BUTTON_1,      GBA_MAP_B      },
    { "A",      WPAD_BUTTON_A,      GBA_MAP_R      },
    { "B",      WPAD_BUTTON_B,      GBA_MAP_L      },
    { NULL,     0,                  GBA_MAP_NONE   },
    { NULL,     0,                  GBA_MAP_NONE   },
    { NULL,     0,                  GBA_MAP_NONE   },
    { NULL,     0,                  GBA_MAP_NONE   }
  },
  {
    { "Plus",   WPAD_BUTTON_PLUS,       GBA_MAP_START  },
    { "Minus",  WPAD_BUTTON_MINUS,      GBA_MAP_SELECT },
    { "2",      WPAD_BUTTON_2,          GBA_MAP_NONE   }, 
    { "1",      WPAD_BUTTON_1,          GBA_MAP_NONE   },
    { "A",      WPAD_BUTTON_A,          GBA_MAP_A      },
    { "B",      WPAD_BUTTON_B,          GBA_MAP_R      },
    { "C",      WPAD_NUNCHUK_BUTTON_C,  GBA_MAP_B      },
    { "Z",      WPAD_NUNCHUK_BUTTON_Z,  GBA_MAP_L      },
    { NULL,     0,                      GBA_MAP_NONE   },
    { NULL,     0,                      GBA_MAP_NONE   }
  },
  {
    { "Plus",   WPAD_CLASSIC_BUTTON_PLUS,     GBA_MAP_START  },
    { "Minus",  WPAD_CLASSIC_BUTTON_MINUS,    GBA_MAP_SELECT },
    { "A",      WPAD_CLASSIC_BUTTON_A,        GBA_MAP_A      }, 
    { "B",      WPAD_CLASSIC_BUTTON_B,        GBA_MAP_B      },
    { "X",      WPAD_CLASSIC_BUTTON_X,        GBA_MAP_NONE   },
    { "Y",      WPAD_CLASSIC_BUTTON_Y,        GBA_MAP_NONE   },
    { "R",      WPAD_CLASSIC_BUTTON_FULL_R,   GBA_MAP_R      },
    { "L",      WPAD_CLASSIC_BUTTON_FULL_L,   GBA_MAP_L      },
    { "zR",     WPAD_CLASSIC_BUTTON_ZR,       GBA_MAP_R      },
    { "zL",     WPAD_CLASSIC_BUTTON_ZL,       GBA_MAP_L      }
  },
  {
    { "Start",  PAD_BUTTON_START,             GBA_MAP_START  },
    { "A",      PAD_BUTTON_A,                 GBA_MAP_A      },
    { "B",      PAD_BUTTON_B,                 GBA_MAP_B      }, 
    { "X",      PAD_BUTTON_X,                 GBA_MAP_SELECT },
    { "Y",      PAD_BUTTON_Y,                 GBA_MAP_SELECT },
    { "R",      PAD_TRIGGER_R,                GBA_MAP_R      },
    { "L",      PAD_TRIGGER_L,                GBA_MAP_L      },
    { NULL,     0,                            GBA_MAP_NONE   },
    { NULL,     0,                            GBA_MAP_NONE   },
    { NULL,     0,                            GBA_MAP_NONE   },
  }
};

GameBoyAdvanceDbManager::GameBoyAdvanceDbManager( Emulator &emulator ) :
  StandardDatabaseManager( emulator )
{
}

dbEntry* GameBoyAdvanceDbManager::getEntry()
{
  return (dbEntry*)&m_entry;
}

void GameBoyAdvanceDbManager::resetToDefaults()
{
  GameBoyAdvance& emu = (GameBoyAdvance&)getEmulator();
  GameBoyAdvanceDbEntry* entry = (GameBoyAdvanceDbEntry*)getEntry();
  memset( entry, 0x0, sizeof( GameBoyAdvanceDbEntry ) );

  StandardDatabaseManager::resetToDefaults();

  resetButtons();
  applyButtonMap();
}

int GameBoyAdvanceDbManager::getMappableButtonCount( int profile )
{
  return buttonCount;
}

const MappableButton* GameBoyAdvanceDbManager::getMappableButton( int profile, int button )
{
  return &GBA_BUTTONS[button];
}

const WiiButton* GameBoyAdvanceDbManager::getMappedButton(
  int profile, int controller, int button )
{
  return &(WII_BUTTONS[controller][button]);
}
