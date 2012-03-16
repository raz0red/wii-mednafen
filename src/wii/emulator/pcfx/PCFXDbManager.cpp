#include "main.h"
#include "PCFX.h"
#include "PCFXDbManager.h"
#include <wiiuse/wpad.h>

// PCE Keys
#define PCFX_MAP_NONE   KEY_MAP_NONE
#define PCFX_MAP_I      1
#define PCFX_MAP_II     2
#define PCFX_MAP_III    3
#define PCFX_MAP_IV     4
#define PCFX_MAP_V      5
#define PCFX_MAP_VI     6
#define PCFX_MAP_RUN    7
#define PCFX_MAP_SELECT 8
#define PCFX_MAP_LEFT   9
#define PCFX_MAP_RIGHT  10
#define PCFX_MAP_UP     11
#define PCFX_MAP_DOWN   12

const MappableButton PCFXDbManager::PCFX_BUTTONS[PCFX_BUTTON_COUNT] = 
{
  { "(none)",   PCFX_NONE    },
  { "I",        PCFX_I       }, 
  { "II",       PCFX_II      }, 
  { "III",      PCFX_III     },
  { "IV",       PCFX_IV      },
  { "V",        PCFX_V       },
  { "VI",       PCFX_VI      },
  { "Run",      PCFX_RUN     },
  { "Select",   PCFX_SELECT  },
  { "Left",     PCFX_LEFT    },
  { "Right",    PCFX_RIGHT   },
  { "Up",       PCFX_UP      },
  { "Down",     PCFX_DOWN    }
};

const WiiButton PCFXDbManager::
  WII_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT] =
{
  {
    { "Plus",   WPAD_BUTTON_PLUS,   PCFX_MAP_RUN    },
    { "Minus",  WPAD_BUTTON_MINUS,  PCFX_MAP_SELECT },
    { "2",      WPAD_BUTTON_2,      PCFX_MAP_I      }, 
    { "1",      WPAD_BUTTON_1,      PCFX_MAP_II     },
    { "A",      WPAD_BUTTON_A,      PCFX_MAP_NONE   },
    { "B",      WPAD_BUTTON_B,      PCFX_MAP_NONE   },
    { NULL,     0,                  PCFX_MAP_NONE   },
    { NULL,     0,                  PCFX_MAP_NONE   },
    { NULL,     0,                  PCFX_MAP_NONE   },
    { NULL,     0,                  PCFX_MAP_NONE   }
  },
  {
    { "Plus",   WPAD_BUTTON_PLUS,       PCFX_MAP_RUN    },
    { "Minus",  WPAD_BUTTON_MINUS,      PCFX_MAP_SELECT },
    { "2",      WPAD_BUTTON_2,          PCFX_MAP_NONE   }, 
    { "1",      WPAD_BUTTON_1,          PCFX_MAP_NONE   },
    { "A",      WPAD_BUTTON_A,          PCFX_MAP_I      },
    { "B",      WPAD_BUTTON_B,          PCFX_MAP_NONE   },
    { "C",      WPAD_NUNCHUK_BUTTON_C,  PCFX_MAP_II     },
    { "Z",      WPAD_NUNCHUK_BUTTON_Z,  PCFX_MAP_NONE   },
    { NULL,     0,                      PCFX_MAP_NONE   },
    { NULL,     0,                      PCFX_MAP_NONE   }
  },
  {
    { "Plus",   WPAD_CLASSIC_BUTTON_PLUS,     PCFX_MAP_RUN    },
    { "Minus",  WPAD_CLASSIC_BUTTON_MINUS,    PCFX_MAP_SELECT },
    { "A",      WPAD_CLASSIC_BUTTON_A,        PCFX_MAP_I      }, 
    { "B",      WPAD_CLASSIC_BUTTON_B,        PCFX_MAP_II     },
    { "X",      WPAD_CLASSIC_BUTTON_X,        PCFX_MAP_V      },
    { "Y",      WPAD_CLASSIC_BUTTON_Y,        PCFX_MAP_III    },
    { "R",      WPAD_CLASSIC_BUTTON_FULL_R,   PCFX_MAP_VI     },
    { "L",      WPAD_CLASSIC_BUTTON_FULL_L,   PCFX_MAP_IV     },
    { "zR",     WPAD_CLASSIC_BUTTON_ZR,       PCFX_MAP_VI     },
    { "zL",     WPAD_CLASSIC_BUTTON_ZL,       PCFX_MAP_IV     }
  },
  {
    { "Start",  PAD_BUTTON_START,             PCFX_MAP_RUN    },
    { "A",      PAD_BUTTON_A,                 PCFX_MAP_I      },
    { "B",      PAD_BUTTON_B,                 PCFX_MAP_II     }, 
    { "X",      PAD_BUTTON_X,                 PCFX_MAP_SELECT },
    { "Y",      PAD_BUTTON_Y,                 PCFX_MAP_SELECT },
    { "R",      PAD_TRIGGER_R,                PCFX_MAP_NONE   },
    { "L",      PAD_TRIGGER_L,                PCFX_MAP_NONE   },
    { NULL,     0,                            PCFX_MAP_NONE   },
    { NULL,     0,                            PCFX_MAP_NONE   },
    { NULL,     0,                            PCFX_MAP_NONE   },
  }
};

PCFXDbManager::PCFXDbManager( Emulator &emulator ) :
  StandardDatabaseManager( emulator )
{
}

dbEntry* PCFXDbManager::getEntry()
{
  return (dbEntry*)&m_entry;
}

void PCFXDbManager::resetToDefaults()
{
  PCFX& emu = (PCFX&)getEmulator();
  PCFXDbEntry* entry = (PCFXDbEntry*)getEntry();
  memset( entry, 0x0, sizeof( PCFXDbEntry ) );

  StandardDatabaseManager::resetToDefaults();

  resetButtons();
  applyButtonMap();
}

int PCFXDbManager::getMappableButtonCount( int profile )
{
  return PCFX_BUTTON_COUNT;
}

const MappableButton* PCFXDbManager::getMappableButton( int profile, int button )
{
  return &PCFX_BUTTONS[button];
}

const WiiButton* PCFXDbManager::getMappedButton(
  int profile, int controller, int button )
{
  return &(WII_BUTTONS[controller][button]);
}
