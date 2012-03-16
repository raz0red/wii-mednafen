#include "main.h"
#include "MasterSystem.h"
#include "MasterSystemDbManager.h"
#include <wiiuse/wpad.h>

#define SMS_MAP_NONE     KEY_MAP_NONE
#define SMS_MAP_FIRE1    1
#define SMS_MAP_FIRE2    2
#define SMS_MAP_PAUSE    3
#define SMS_MAP_LEFT     4
#define SMS_MAP_RIGHT    5
#define SMS_MAP_UP       6
#define SMS_MAP_DOWN     7
#define SMS_MAP_FIRE1_R  8
#define SMS_MAP_FIRE2_R  9
#define SMS_MAP_REWIND   10

const MappableButton MasterSystemDbManager::SMS_BUTTONS[] = 
{
  { "(none)",       SMS_NONE      },
  { "Fire 1/Start", SMS_FIRE1     }, 
  { "Fire 2",       SMS_FIRE2     }, 
  { "Pause",        SMS_PAUSE     },
  { "Left",         SMS_LEFT      },
  { "Right",        SMS_RIGHT     },
  { "Up",           SMS_UP        },
  { "Down",         SMS_DOWN      },
  { "Fire 1",       SMS_FIRE1_R   }, 
  { "Fire 2",       SMS_FIRE2_R   },
  { "(rewind)",     SMS_REWIND    } 
};

static const int buttonCount =  
  sizeof(MasterSystemDbManager::SMS_BUTTONS)/sizeof(MappableButton);

const WiiButton MasterSystemDbManager::
  WII_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT] =
{
  {
    { "Plus",   WPAD_BUTTON_PLUS,   SMS_MAP_PAUSE  },
    { "Minus",  WPAD_BUTTON_MINUS,  SMS_MAP_NONE   },
    { "2",      WPAD_BUTTON_2,      SMS_MAP_FIRE2  }, 
    { "1",      WPAD_BUTTON_1,      SMS_MAP_FIRE1  },
    { "A",      WPAD_BUTTON_A,      SMS_MAP_NONE   },
    { "B",      WPAD_BUTTON_B,      SMS_MAP_NONE   },
    { NULL,     0,                  SMS_MAP_NONE   },
    { NULL,     0,                  SMS_MAP_NONE   },
    { NULL,     0,                  SMS_MAP_NONE   },
    { NULL,     0,                  SMS_MAP_NONE   }
  },
  {
    { "Plus",   WPAD_BUTTON_PLUS,       SMS_MAP_PAUSE  },
    { "Minus",  WPAD_BUTTON_MINUS,      SMS_MAP_NONE   },
    { "2",      WPAD_BUTTON_2,          SMS_MAP_NONE   }, 
    { "1",      WPAD_BUTTON_1,          SMS_MAP_NONE   },
    { "A",      WPAD_BUTTON_A,          SMS_MAP_FIRE2  },
    { "B",      WPAD_BUTTON_B,          SMS_MAP_NONE   },
    { "C",      WPAD_NUNCHUK_BUTTON_C,  SMS_MAP_FIRE1  },
    { "Z",      WPAD_NUNCHUK_BUTTON_Z,  SMS_MAP_NONE   },
    { NULL,     0,                      SMS_MAP_NONE   },
    { NULL,     0,                      SMS_MAP_NONE   }
  },
  {
    { "Plus",   WPAD_CLASSIC_BUTTON_PLUS,     SMS_MAP_PAUSE  },
    { "Minus",  WPAD_CLASSIC_BUTTON_MINUS,    SMS_MAP_NONE   },
    { "A",      WPAD_CLASSIC_BUTTON_A,        SMS_MAP_FIRE2  }, 
    { "B",      WPAD_CLASSIC_BUTTON_B,        SMS_MAP_FIRE1  },
    { "X",      WPAD_CLASSIC_BUTTON_X,        SMS_MAP_NONE   },
    { "Y",      WPAD_CLASSIC_BUTTON_Y,        SMS_MAP_NONE   },
    { "R",      WPAD_CLASSIC_BUTTON_FULL_R,   SMS_MAP_NONE   },
    { "L",      WPAD_CLASSIC_BUTTON_FULL_L,   SMS_MAP_NONE   },
    { "zR",     WPAD_CLASSIC_BUTTON_ZR,       SMS_MAP_NONE   },
    { "zL",     WPAD_CLASSIC_BUTTON_ZL,       SMS_MAP_NONE   }
  },
  {
    { "Start",  PAD_BUTTON_START,             SMS_MAP_PAUSE  },
    { "A",      PAD_BUTTON_A,                 SMS_MAP_FIRE2  },
    { "B",      PAD_BUTTON_B,                 SMS_MAP_FIRE1  }, 
    { "X",      PAD_BUTTON_X,                 SMS_MAP_NONE   },
    { "Y",      PAD_BUTTON_Y,                 SMS_MAP_NONE   },
    { "R",      PAD_TRIGGER_R,                SMS_MAP_NONE   },
    { "L",      PAD_TRIGGER_L,                SMS_MAP_NONE   },
    { NULL,     0,                            SMS_MAP_NONE   },
    { NULL,     0,                            SMS_MAP_NONE   },
    { NULL,     0,                            SMS_MAP_NONE   },
  }
};

MasterSystemDbManager::MasterSystemDbManager( Emulator &emulator ) :
  StandardDatabaseManager( emulator )
{
}

dbEntry* MasterSystemDbManager::getEntry()
{
  return (dbEntry*)&m_entry;
}

void MasterSystemDbManager::resetToDefaults()
{
  MasterSystem& emu = (MasterSystem&)getEmulator();
  MasterSystemDbEntry* entry = (MasterSystemDbEntry*)getEntry();
  memset( entry, 0x0, sizeof( MasterSystemDbEntry ) );

  StandardDatabaseManager::resetToDefaults();

  resetButtons();
  applyButtonMap();
}

int MasterSystemDbManager::getMappableButtonCount( int profile )
{
  return buttonCount;
}

const MappableButton* MasterSystemDbManager::getMappableButton( int profile, int button )
{
  return &SMS_BUTTONS[button];
}

const WiiButton* MasterSystemDbManager::getMappedButton(
  int profile, int controller, int button )
{
  return &(WII_BUTTONS[controller][button]);
}
