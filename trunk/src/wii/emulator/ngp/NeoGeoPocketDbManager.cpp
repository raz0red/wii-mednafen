#include "main.h"
#include "NeoGeoPocket.h"
#include "NeoGeoPocketDbManager.h"
#include <wiiuse/wpad.h>

#define NGP_MAP_NONE    KEY_MAP_NONE
#define NGP_MAP_A       1
#define NGP_MAP_B       2
#define NGP_MAP_OPTION  3
#define NGP_MAP_LEFT    4
#define NGP_MAP_RIGHT   5
#define NGP_MAP_UP      6
#define NGP_MAP_DOWN    7
#define NGP_MAP_A_R     8
#define NGP_MAP_B_R     9
#define NGP_MAP_REWIND  10

const MappableButton NeoGeoPocketDbManager::NGP_BUTTONS[] = 
{
  { "(none)",   NGP_NONE    },
  { "A",        NGP_A       }, 
  { "B",        NGP_B       }, 
  { "Option",   NGP_OPTION  },
  { "Left",     NGP_LEFT    },
  { "Right",    NGP_RIGHT   },
  { "Up",       NGP_UP      },
  { "Down",     NGP_DOWN    },
  { "A",        NGP_A_R     }, 
  { "B",        NGP_B_R     },
  { "(rewind)", NGP_REWIND  }
};

static const int buttonCount =  
  sizeof(NeoGeoPocketDbManager::NGP_BUTTONS)/sizeof(MappableButton);

const WiiButton NeoGeoPocketDbManager::
  WII_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT] =
{
  {
    { "Plus",   WPAD_BUTTON_PLUS,   NGP_MAP_OPTION },
    { "Minus",  WPAD_BUTTON_MINUS,  NGP_MAP_NONE   },
    { "2",      WPAD_BUTTON_2,      NGP_MAP_A      }, 
    { "1",      WPAD_BUTTON_1,      NGP_MAP_B      },
    { "A",      WPAD_BUTTON_A,      NGP_MAP_NONE   },
    { "B",      WPAD_BUTTON_B,      NGP_MAP_NONE   },
    { NULL,     0,                  NGP_MAP_NONE   },
    { NULL,     0,                  NGP_MAP_NONE   },
    { NULL,     0,                  NGP_MAP_NONE   },
    { NULL,     0,                  NGP_MAP_NONE   }
  },
  {
    { "Plus",   WPAD_BUTTON_PLUS,       NGP_MAP_OPTION },
    { "Minus",  WPAD_BUTTON_MINUS,      NGP_MAP_NONE   },
    { "2",      WPAD_BUTTON_2,          NGP_MAP_NONE   }, 
    { "1",      WPAD_BUTTON_1,          NGP_MAP_NONE   },
    { "A",      WPAD_BUTTON_A,          NGP_MAP_A      },
    { "B",      WPAD_BUTTON_B,          NGP_MAP_NONE   },
    { "C",      WPAD_NUNCHUK_BUTTON_C,  NGP_MAP_B      },
    { "Z",      WPAD_NUNCHUK_BUTTON_Z,  NGP_MAP_NONE   },
    { NULL,     0,                      NGP_MAP_NONE   },
    { NULL,     0,                      NGP_MAP_NONE   }
  },
  {
    { "Plus",   WPAD_CLASSIC_BUTTON_PLUS,     NGP_MAP_OPTION },
    { "Minus",  WPAD_CLASSIC_BUTTON_MINUS,    NGP_MAP_NONE   },
    { "A",      WPAD_CLASSIC_BUTTON_A,        NGP_MAP_A      }, 
    { "B",      WPAD_CLASSIC_BUTTON_B,        NGP_MAP_B      },
    { "X",      WPAD_CLASSIC_BUTTON_X,        NGP_MAP_NONE   },
    { "Y",      WPAD_CLASSIC_BUTTON_Y,        NGP_MAP_NONE   },
    { "R",      WPAD_CLASSIC_BUTTON_FULL_R,   NGP_MAP_NONE   },
    { "L",      WPAD_CLASSIC_BUTTON_FULL_L,   NGP_MAP_NONE   },
    { "zR",     WPAD_CLASSIC_BUTTON_ZR,       NGP_MAP_NONE   },
    { "zL",     WPAD_CLASSIC_BUTTON_ZL,       NGP_MAP_NONE   }
  },
  {
    { "Start",  PAD_BUTTON_START,             NGP_MAP_OPTION },
    { "A",      PAD_BUTTON_A,                 NGP_MAP_A      },
    { "B",      PAD_BUTTON_B,                 NGP_MAP_B      }, 
    { "X",      PAD_BUTTON_X,                 NGP_MAP_NONE   },
    { "Y",      PAD_BUTTON_Y,                 NGP_MAP_NONE   },
    { "R",      PAD_TRIGGER_R,                NGP_MAP_NONE   },
    { "L",      PAD_TRIGGER_L,                NGP_MAP_NONE   },
    { NULL,     0,                            NGP_MAP_NONE   },
    { NULL,     0,                            NGP_MAP_NONE   },
    { NULL,     0,                            NGP_MAP_NONE   },
  }
};

NeoGeoPocketDbManager::NeoGeoPocketDbManager( Emulator &emulator ) :
  StandardDatabaseManager( emulator )
{
}

dbEntry* NeoGeoPocketDbManager::getEntry()
{
  return (dbEntry*)&m_entry;
}

void NeoGeoPocketDbManager::resetToDefaults()
{
  NeoGeoPocket& emu = (NeoGeoPocket&)getEmulator();
  NeoGeoPocketDbEntry* entry = (NeoGeoPocketDbEntry*)getEntry();
  memset( entry, 0x0, sizeof( NeoGeoPocketDbEntry ) );

  StandardDatabaseManager::resetToDefaults();

  resetButtons();
  applyButtonMap();
}

int NeoGeoPocketDbManager::getMappableButtonCount( int profile )
{
  return buttonCount;
}

const MappableButton* NeoGeoPocketDbManager::getMappableButton( int profile, int button )
{
  return &NGP_BUTTONS[button];
}

const WiiButton* NeoGeoPocketDbManager::getMappedButton(
  int profile, int controller, int button )
{
  return &(WII_BUTTONS[controller][button]);
}
