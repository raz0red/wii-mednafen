#include "main.h"
#include "WonderSwan.h"
#include <wiiuse/wpad.h>

#define WSK_MAP_NONE    KEY_MAP_NONE
#define WSK_MAP_A       1
#define WSK_MAP_B       2
#define WSK_MAP_START   3
#define WSK_MAP_X1      4
#define WSK_MAP_X2      5
#define WSK_MAP_X3      6
#define WSK_MAP_X4      7
#define WSK_MAP_Y1      8
#define WSK_MAP_Y2      9
#define WSK_MAP_Y3      10
#define WSK_MAP_Y4      11
#define WSK_MAP_A_R     12
#define WSK_MAP_B_R     13
#define WSK_MAP_ROT     14
#define WSK_MAP_REW     15

const MappableButton WonderSwanDbManager::WS_BUTTONS[] = 
{
  { "(none)",           WS_NONE   },
  { "A",                WS_A      }, 
  { "B",                WS_B      }, 
  { "Start",            WS_START  },
  { "X1",               WS_X1     },
  { "X2",               WS_X2     },
  { "X3",               WS_X3     },
  { "X4",               WS_X4     },
  { "Y1",               WS_Y1     },
  { "Y2",               WS_Y2     },
  { "Y3",               WS_Y3     },
  { "Y4",               WS_Y4     },
  { "A",                WS_A_R    }, 
  { "B",                WS_B_R    },
  { "(rotate screen)",  WS_ROTATE },
  { "(rewind)",         WS_REWIND } 
};

static const int buttonCount =  
  sizeof(WonderSwanDbManager::WS_BUTTONS)/sizeof(MappableButton);

const WiiButton WonderSwanDbManager::
  WII_BUTTONS[WS_PROFILE_COUNT][WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT] =
{
  { // Profile normal
    {
      { "Plus",   WPAD_BUTTON_PLUS,   WSK_MAP_START  },
      { "Minus",  WPAD_BUTTON_MINUS,  WSK_MAP_Y3     },
      { "2",      WPAD_BUTTON_2,      WSK_MAP_A      }, 
      { "1",      WPAD_BUTTON_1,      WSK_MAP_B      },
      { "A",      WPAD_BUTTON_A,      WSK_MAP_Y4     },
      { "B",      WPAD_BUTTON_B,      WSK_MAP_Y1     },
      { NULL,     0,                  WSK_MAP_NONE   },
      { NULL,     0,                  WSK_MAP_NONE   },
      { NULL,     0,                  WSK_MAP_NONE   },
      { NULL,     0,                  WSK_MAP_NONE   }
    },
    {
      { "Plus",   WPAD_BUTTON_PLUS,       WSK_MAP_START  },
      { "Minus",  WPAD_BUTTON_MINUS,      WSK_MAP_Y3     },
      { "2",      WPAD_BUTTON_2,          WSK_MAP_Y3     }, 
      { "1",      WPAD_BUTTON_1,          WSK_MAP_Y2     },
      { "A",      WPAD_BUTTON_A,          WSK_MAP_A      },
      { "B",      WPAD_BUTTON_B,          WSK_MAP_Y4     },
      { "C",      WPAD_NUNCHUK_BUTTON_C,  WSK_MAP_B      },
      { "Z",      WPAD_NUNCHUK_BUTTON_Z,  WSK_MAP_Y1     },
      { NULL,     0,                      WSK_MAP_NONE   },
      { NULL,     0,                      WSK_MAP_NONE   }
    },
    {
      { "Plus",   WPAD_CLASSIC_BUTTON_PLUS,     WSK_MAP_START  },
      { "Minus",  WPAD_CLASSIC_BUTTON_MINUS,    WSK_MAP_NONE   },
      { "A",      WPAD_CLASSIC_BUTTON_A,        WSK_MAP_A      }, 
      { "B",      WPAD_CLASSIC_BUTTON_B,        WSK_MAP_B      },
      { "X",      WPAD_CLASSIC_BUTTON_X,        WSK_MAP_Y2     },
      { "Y",      WPAD_CLASSIC_BUTTON_Y,        WSK_MAP_Y3     },
      { "R",      WPAD_CLASSIC_BUTTON_FULL_R,   WSK_MAP_Y1     },
      { "L",      WPAD_CLASSIC_BUTTON_FULL_L,   WSK_MAP_Y4     },
      { "zR",     WPAD_CLASSIC_BUTTON_ZR,       WSK_MAP_Y1     },
      { "zL",     WPAD_CLASSIC_BUTTON_ZL,       WSK_MAP_Y4     }
    },
    {
      { "Start",  PAD_BUTTON_START,             WSK_MAP_START  },
      { "A",      PAD_BUTTON_A,                 WSK_MAP_A      },
      { "B",      PAD_BUTTON_B,                 WSK_MAP_B      }, 
      { "X",      PAD_BUTTON_X,                 WSK_MAP_Y2     },
      { "Y",      PAD_BUTTON_Y,                 WSK_MAP_Y3     },
      { "R",      PAD_TRIGGER_R,                WSK_MAP_Y1     },
      { "L",      PAD_TRIGGER_L,                WSK_MAP_Y4     },
      { NULL,     0,                            WSK_MAP_NONE   },
      { NULL,     0,                            WSK_MAP_NONE   },
      { NULL,     0,                            WSK_MAP_NONE   },
    }
  },
  { // Profile rotated
    {
      { "Plus",   WPAD_BUTTON_PLUS,   WSK_MAP_START  },
      { "Minus",  WPAD_BUTTON_MINUS,  WSK_MAP_B      },
      { "2",      WPAD_BUTTON_2,      WSK_MAP_X2     }, 
      { "1",      WPAD_BUTTON_1,      WSK_MAP_X3     },
      { "A",      WPAD_BUTTON_A,      WSK_MAP_X4     },
      { "B",      WPAD_BUTTON_B,      WSK_MAP_X1     },
      { NULL,     0,                  WSK_MAP_NONE   },
      { NULL,     0,                  WSK_MAP_NONE   },
      { NULL,     0,                  WSK_MAP_NONE   },
      { NULL,     0,                  WSK_MAP_NONE   }
    },
    {
      { "Plus",   WPAD_BUTTON_PLUS,       WSK_MAP_START },
      { "Minus",  WPAD_BUTTON_MINUS,      WSK_MAP_B     },
      { "2",      WPAD_BUTTON_2,          WSK_MAP_A     }, 
      { "1",      WPAD_BUTTON_1,          WSK_MAP_B     },
      { "A",      WPAD_BUTTON_A,          WSK_MAP_X2    },
      { "B",      WPAD_BUTTON_B,          WSK_MAP_X4    },
      { "C",      WPAD_NUNCHUK_BUTTON_C,  WSK_MAP_X3    },
      { "Z",      WPAD_NUNCHUK_BUTTON_Z,  WSK_MAP_X1    },
      { NULL,     0,                      WSK_MAP_NONE  },
      { NULL,     0,                      WSK_MAP_NONE  }
    },
    {
      { "Plus",   WPAD_CLASSIC_BUTTON_PLUS,     WSK_MAP_START  },
      { "Minus",  WPAD_CLASSIC_BUTTON_MINUS,    WSK_MAP_NONE   },
      { "A",      WPAD_CLASSIC_BUTTON_A,        WSK_MAP_X3     }, 
      { "B",      WPAD_CLASSIC_BUTTON_B,        WSK_MAP_X4     },
      { "X",      WPAD_CLASSIC_BUTTON_X,        WSK_MAP_X2     },
      { "Y",      WPAD_CLASSIC_BUTTON_Y,        WSK_MAP_X1     },
      { "R",      WPAD_CLASSIC_BUTTON_FULL_R,   WSK_MAP_A      },
      { "L",      WPAD_CLASSIC_BUTTON_FULL_L,   WSK_MAP_B      },
      { "zR",     WPAD_CLASSIC_BUTTON_ZR,       WSK_MAP_A      },
      { "zL",     WPAD_CLASSIC_BUTTON_ZL,       WSK_MAP_B      }
    },
    {
      { "Start",  PAD_BUTTON_START,             WSK_MAP_START  },
      { "A",      PAD_BUTTON_A,                 WSK_MAP_X3     },
      { "B",      PAD_BUTTON_B,                 WSK_MAP_X4     }, 
      { "X",      PAD_BUTTON_X,                 WSK_MAP_X2     },
      { "Y",      PAD_BUTTON_Y,                 WSK_MAP_X1     },
      { "R",      PAD_TRIGGER_R,                WSK_MAP_A      },
      { "L",      PAD_TRIGGER_L,                WSK_MAP_B      },
      { NULL,     0,                            WSK_MAP_NONE   },
      { NULL,     0,                            WSK_MAP_NONE   },
      { NULL,     0,                            WSK_MAP_NONE   },
    }
  }
};

static const char* profileNames[WS_PROFILE_COUNT] =
{
  "Normal", "Rotated"
};

WonderSwanDbManager::WonderSwanDbManager( Emulator &emulator ) :
  StandardDatabaseManager( emulator )
{
}

dbEntry* WonderSwanDbManager::getEntry()
{
  return (dbEntry*)&m_entry;
}

void WonderSwanDbManager::resetToDefaults()
{
  WonderSwan& emu = (WonderSwan&)getEmulator();
  WswanDbEntry* entry = (WswanDbEntry*)getEntry();
  memset( entry, 0x0, sizeof( WswanDbEntry ) );

  StandardDatabaseManager::resetToDefaults();

  entry->base.profile = MDFNGameInfo->rotated ? 
    WS_PROFILE_ROTATED90 : WS_PROFILE_NORMAL;
  resetButtons();
  applyButtonMap();
}

int WonderSwanDbManager::getMappableButtonCount( int profile )
{
  return buttonCount;
}

const MappableButton* WonderSwanDbManager::getMappableButton( int profile, int button )
{
  return &WS_BUTTONS[button];
}

const WiiButton* WonderSwanDbManager::getMappedButton(
  int profile, int controller, int button )
{
  return &(WII_BUTTONS[profile][controller][button]);
}

int WonderSwanDbManager::getProfileCount()
{
  return WS_PROFILE_COUNT;
}

const char* WonderSwanDbManager::getProfileName( int profile )
{
  return profileNames[profile];
}