#include "main.h"
#include "GameGear.h"
#include "GameGearDbManager.h"
#include <wiiuse/wpad.h>

#define GG_MAP_NONE     KEY_MAP_NONE
#define GG_MAP_BTN1     1
#define GG_MAP_BTN2     2
#define GG_MAP_START    3
#define GG_MAP_LEFT     4
#define GG_MAP_RIGHT    5
#define GG_MAP_UP       6
#define GG_MAP_DOWN     7
#define GG_MAP_BTN1_R   8
#define GG_MAP_BTN2_R   9
#define GG_MAP_REWIND   10

const MappableButton GameGearDbManager::GG_BUTTONS[] = 
{
  { "(none)",   GG_NONE    },
  { "Button 1", GG_BTN1    }, 
  { "Button 2", GG_BTN2    }, 
  { "Start",    GG_START   },
  { "Left",     GG_LEFT    },
  { "Right",    GG_RIGHT   },
  { "Up",       GG_UP      },
  { "Down",     GG_DOWN    },
  { "Button 1", GG_BTN1_R  }, 
  { "Button 2", GG_BTN2_R  },
  { "(rewind)", GG_REWIND  }  
};

static const int buttonCount =  
  sizeof(GameGearDbManager::GG_BUTTONS)/sizeof(MappableButton);

const WiiButton GameGearDbManager::
  WII_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT] =
{
  {
    { "Plus",   WPAD_BUTTON_PLUS,   GG_MAP_START  },
    { "Minus",  WPAD_BUTTON_MINUS,  GG_MAP_NONE   },
    { "2",      WPAD_BUTTON_2,      GG_MAP_BTN2   }, 
    { "1",      WPAD_BUTTON_1,      GG_MAP_BTN1   },
    { "A",      WPAD_BUTTON_A,      GG_MAP_NONE   },
    { "B",      WPAD_BUTTON_B,      GG_MAP_NONE   },
    { NULL,     0,                  GG_MAP_NONE   },
    { NULL,     0,                  GG_MAP_NONE   },
    { NULL,     0,                  GG_MAP_NONE   },
    { NULL,     0,                  GG_MAP_NONE   }
  },
  {
    { "Plus",   WPAD_BUTTON_PLUS,       GG_MAP_START  },
    { "Minus",  WPAD_BUTTON_MINUS,      GG_MAP_NONE   },
    { "2",      WPAD_BUTTON_2,          GG_MAP_NONE   }, 
    { "1",      WPAD_BUTTON_1,          GG_MAP_NONE   },
    { "A",      WPAD_BUTTON_A,          GG_MAP_BTN2   },
    { "B",      WPAD_BUTTON_B,          GG_MAP_NONE   },
    { "C",      WPAD_NUNCHUK_BUTTON_C,  GG_MAP_BTN1   },
    { "Z",      WPAD_NUNCHUK_BUTTON_Z,  GG_MAP_NONE   },
    { NULL,     0,                      GG_MAP_NONE   },
    { NULL,     0,                      GG_MAP_NONE   }
  },
  {
    { "Plus",   WPAD_CLASSIC_BUTTON_PLUS,     GG_MAP_START  },
    { "Minus",  WPAD_CLASSIC_BUTTON_MINUS,    GG_MAP_NONE   },
    { "A",      WPAD_CLASSIC_BUTTON_A,        GG_MAP_BTN2   }, 
    { "B",      WPAD_CLASSIC_BUTTON_B,        GG_MAP_BTN1   },
    { "X",      WPAD_CLASSIC_BUTTON_X,        GG_MAP_NONE   },
    { "Y",      WPAD_CLASSIC_BUTTON_Y,        GG_MAP_NONE   },
    { "R",      WPAD_CLASSIC_BUTTON_FULL_R,   GG_MAP_NONE   },
    { "L",      WPAD_CLASSIC_BUTTON_FULL_L,   GG_MAP_NONE   },
    { "zR",     WPAD_CLASSIC_BUTTON_ZR,       GG_MAP_NONE   },
    { "zL",     WPAD_CLASSIC_BUTTON_ZL,       GG_MAP_NONE   }
  },
  {
    { "Start",  PAD_BUTTON_START,             GG_MAP_START  },
    { "A",      PAD_BUTTON_A,                 GG_MAP_BTN2   },
    { "B",      PAD_BUTTON_B,                 GG_MAP_BTN1   }, 
    { "X",      PAD_BUTTON_X,                 GG_MAP_NONE   },
    { "Y",      PAD_BUTTON_Y,                 GG_MAP_NONE   },
    { "R",      PAD_TRIGGER_R,                GG_MAP_NONE   },
    { "L",      PAD_TRIGGER_L,                GG_MAP_NONE   },
    { NULL,     0,                            GG_MAP_NONE   },
    { NULL,     0,                            GG_MAP_NONE   },
    { NULL,     0,                            GG_MAP_NONE   },
  }
};

GameGearDbManager::GameGearDbManager( Emulator &emulator ) :
  StandardDatabaseManager( emulator )
{
}

dbEntry* GameGearDbManager::getEntry()
{
  return (dbEntry*)&m_entry;
}

void GameGearDbManager::resetToDefaults()
{
  GameGear& emu = (GameGear&)getEmulator();
  GameGearDbEntry* entry = (GameGearDbEntry*)getEntry();
  memset( entry, 0x0, sizeof( GameGearDbEntry ) );

  StandardDatabaseManager::resetToDefaults();

  resetButtons();
  applyButtonMap();
}

int GameGearDbManager::getMappableButtonCount( int profile )
{
  return buttonCount;
}

const MappableButton* GameGearDbManager::getMappableButton( int profile, int button )
{
  return &GG_BUTTONS[button];
}

const WiiButton* GameGearDbManager::getMappedButton(
  int profile, int controller, int button )
{
  return &(WII_BUTTONS[controller][button]);
}
