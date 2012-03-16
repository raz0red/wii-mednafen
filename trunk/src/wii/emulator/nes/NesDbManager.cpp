#include "main.h"
#include "Nes.h"
#include "NesDbManager.h"
#include <wiiuse/wpad.h>

#ifdef WII_NETTRACE
#include <network.h> 
#include "net_print.h"  
#endif

// Profile Gamepads
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
#define NES_MAP_REWIND  12

// Profile Zapper (in addition to gamepad)
#define NES_MAP_ZAP       13
#define NES_MAP_ZAP_AWAY  14

// Profile Arkanoid (in addition to gamepad)
#define NES_MAP_ARK_BTN   13

// Profile Space Shadow Gun (in addition to gamepad)
#define NES_MAP_FIRE      13
#define NES_MAP_GRENADE   14

// Profile Oeka Kids (in addition to gamepad)
#define NES_MAP_OEKA_BTN  13

// Profile Hypershot (in addition to gamepad)
#define NES_MAP_HS_RUN    13
#define NES_MAP_HS_JUMP   14

#define DEFAULT_NES_BUTTONS          \
  { "(none)",         NES_NONE    }, \
  { "A",              NES_A       }, \
  { "B",              NES_B       }, \
  { "Start",          NES_START   }, \
  { "Select",         NES_SELECT  }, \
  { "Left",           NES_LEFT    }, \
  { "Right",          NES_RIGHT   }, \
  { "Up",             NES_UP      }, \
  { "Down",           NES_DOWN    }, \
  { "A",              NES_A_R     }, \
  { "B",              NES_B_R     }, \
  { "(special)",      NES_SPECIAL }, \
  { "(rewind)",       NES_REWIND  }  


const MappableButton NesDbManager::NES_BUTTONS[] = 
{
  DEFAULT_NES_BUTTONS
};

const MappableButton NesDbManager::NES_ZAPPER_BUTTONS[] = 
{
  DEFAULT_NES_BUTTONS,
  { "Zapper trigger",         NES_ZAP_TRIGGER       }, 
  { "Zapper trigger (away)",  NES_ZAP_TRIGGER_AWAY  }
};

const MappableButton NesDbManager::NES_ARKANOID_BUTTONS[] = 
{
  DEFAULT_NES_BUTTONS,
  { "Button", NES_ARK_BUTTON }
};

const MappableButton NesDbManager::NES_SPACE_SHADOW_BUTTONS[] = 
{
  DEFAULT_NES_BUTTONS,
  { "Fire",     NES_ZAP_TRIGGER }, 
  { "Grenade",  NES_B           }
};

const MappableButton NesDbManager::NES_OEKAKIDS_BUTTONS[] = 
{
  DEFAULT_NES_BUTTONS,
  { "Button", NES_ZAP_TRIGGER }
};

const MappableButton NesDbManager::NES_HYPERSHOT_BUTTONS[] = 
{
  DEFAULT_NES_BUTTONS,
  { "Run",   NES_HS_I_RUN  },
  { "Jump",  NES_HS_I_JUMP }
};

static const int buttonCount =  
  sizeof(NesDbManager::NES_BUTTONS)/sizeof(MappableButton);

static const int zapperButtonCount =  
  sizeof(NesDbManager::NES_ZAPPER_BUTTONS)/sizeof(MappableButton);

static const int arkanoidButtonCount =  
  sizeof(NesDbManager::NES_ARKANOID_BUTTONS)/sizeof(MappableButton);

static const int spaceShadowButtonCount =  
  sizeof(NesDbManager::NES_SPACE_SHADOW_BUTTONS)/sizeof(MappableButton);

static const int oekakidsButtonCount =  
  sizeof(NesDbManager::NES_OEKAKIDS_BUTTONS)/sizeof(MappableButton);

static const int hypershotButtonCount =  
  sizeof(NesDbManager::NES_HYPERSHOT_BUTTONS)/sizeof(MappableButton);

#define DEFAULT_NUNCHUK_CONTROLS                           \
    {                                                      \
    { "Plus",   WPAD_BUTTON_PLUS,       NES_MAP_START   }, \
    { "Minus",  WPAD_BUTTON_MINUS,      NES_MAP_SELECT  }, \
    { "2",      WPAD_BUTTON_2,          NES_MAP_SPECIAL }, \
    { "1",      WPAD_BUTTON_1,          NES_MAP_SPECIAL }, \
    { "A",      WPAD_BUTTON_A,          NES_MAP_A       }, \
    { "B",      WPAD_BUTTON_B,          NES_MAP_NONE    }, \
    { "C",      WPAD_NUNCHUK_BUTTON_C,  NES_MAP_B       }, \
    { "Z",      WPAD_NUNCHUK_BUTTON_Z,  NES_MAP_NONE    }, \
    { NULL,     0,                      NES_MAP_NONE    }, \
    { NULL,     0,                      NES_MAP_NONE    }  \
  }

#define DEFAULT_CLASSIC_CONTROLS                                 \
  {                                                              \
    { "Plus",   WPAD_CLASSIC_BUTTON_PLUS,     NES_MAP_START   }, \
    { "Minus",  WPAD_CLASSIC_BUTTON_MINUS,    NES_MAP_SELECT  }, \
    { "A",      WPAD_CLASSIC_BUTTON_A,        NES_MAP_A       }, \
    { "B",      WPAD_CLASSIC_BUTTON_B,        NES_MAP_B       }, \
    { "X",      WPAD_CLASSIC_BUTTON_X,        NES_MAP_NONE    }, \
    { "Y",      WPAD_CLASSIC_BUTTON_Y,        NES_MAP_NONE    }, \
    { "R",      WPAD_CLASSIC_BUTTON_FULL_R,   NES_MAP_SPECIAL }, \
    { "L",      WPAD_CLASSIC_BUTTON_FULL_L,   NES_MAP_SPECIAL }, \
    { "zR",     WPAD_CLASSIC_BUTTON_ZR,       NES_MAP_SPECIAL }, \
    { "zL",     WPAD_CLASSIC_BUTTON_ZL,       NES_MAP_SPECIAL }  \
  }

#define DEFAULT_GAMECUBE_CONTROLS                                \
  {                                                              \
    { "Start",  PAD_BUTTON_START,             NES_MAP_START   }, \
    { "A",      PAD_BUTTON_A,                 NES_MAP_A       }, \
    { "B",      PAD_BUTTON_B,                 NES_MAP_B       }, \
    { "X",      PAD_BUTTON_X,                 NES_MAP_SELECT  }, \
    { "Y",      PAD_BUTTON_Y,                 NES_MAP_SELECT  }, \
    { "R",      PAD_TRIGGER_R,                NES_MAP_SPECIAL }, \
    { "L",      PAD_TRIGGER_L,                NES_MAP_SPECIAL }, \
    { NULL,     0,                            NES_MAP_NONE    }, \
    { NULL,     0,                            NES_MAP_NONE    }, \
    { NULL,     0,                            NES_MAP_NONE    }  \
  }

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
  DEFAULT_NUNCHUK_CONTROLS,
  DEFAULT_CLASSIC_CONTROLS,
  DEFAULT_GAMECUBE_CONTROLS
};

const WiiButton NesDbManager::
  WII_ZAPPER_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT] =
{
  {
    { "Plus",   WPAD_BUTTON_PLUS,   NES_MAP_START   },
    { "Minus",  WPAD_BUTTON_MINUS,  NES_MAP_SELECT  },
    { "2",      WPAD_BUTTON_2,      NES_MAP_A       }, 
    { "1",      WPAD_BUTTON_1,      NES_MAP_B       },
    { "A",      WPAD_BUTTON_A,      NES_MAP_SPECIAL },
    { "B",      WPAD_BUTTON_B,      NES_MAP_ZAP     },
    { NULL,     0,                  NES_MAP_NONE    },
    { NULL,     0,                  NES_MAP_NONE    },
    { NULL,     0,                  NES_MAP_NONE    },
    { NULL,     0,                  NES_MAP_NONE    }
  },
  DEFAULT_NUNCHUK_CONTROLS,
  DEFAULT_CLASSIC_CONTROLS,
  DEFAULT_GAMECUBE_CONTROLS
};

const WiiButton NesDbManager::
  WII_ARKANOID_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT] =
{
  {
    { "Plus",   WPAD_BUTTON_PLUS,   NES_MAP_START    },
    { "Minus",  WPAD_BUTTON_MINUS,  NES_MAP_SELECT   },
    { "2",      WPAD_BUTTON_2,      NES_MAP_ARK_BTN  }, 
    { "1",      WPAD_BUTTON_1,      NES_MAP_B        },
    { "A",      WPAD_BUTTON_A,      NES_MAP_A        },
    { "B",      WPAD_BUTTON_B,      NES_MAP_ARK_BTN  },
    { NULL,     0,                  NES_MAP_NONE     },
    { NULL,     0,                  NES_MAP_NONE     },
    { NULL,     0,                  NES_MAP_NONE     },
    { NULL,     0,                  NES_MAP_NONE     }
  },
  DEFAULT_NUNCHUK_CONTROLS,
  DEFAULT_CLASSIC_CONTROLS,
  DEFAULT_GAMECUBE_CONTROLS
};

const WiiButton NesDbManager::
  WII_SPACE_SHADOW_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT] =
{
  {
    { "Plus",   WPAD_BUTTON_PLUS,   NES_MAP_START   },
    { "Minus",  WPAD_BUTTON_MINUS,  NES_MAP_SELECT  },
    { "2",      WPAD_BUTTON_2,      NES_MAP_A       }, 
    { "1",      WPAD_BUTTON_1,      NES_MAP_B       },
    { "A",      WPAD_BUTTON_A,      NES_MAP_GRENADE },
    { "B",      WPAD_BUTTON_B,      NES_MAP_FIRE    },
    { NULL,     0,                  NES_MAP_NONE    },
    { NULL,     0,                  NES_MAP_NONE    },
    { NULL,     0,                  NES_MAP_NONE    },
    { NULL,     0,                  NES_MAP_NONE    }
  },
  DEFAULT_NUNCHUK_CONTROLS,
  DEFAULT_CLASSIC_CONTROLS,
  DEFAULT_GAMECUBE_CONTROLS  
};

const WiiButton NesDbManager::
  WII_OEKAKIDS_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT] =
{
  {
    { "Plus",   WPAD_BUTTON_PLUS,   NES_MAP_START    },
    { "Minus",  WPAD_BUTTON_MINUS,  NES_MAP_SELECT   },
    { "2",      WPAD_BUTTON_2,      NES_MAP_A        }, 
    { "1",      WPAD_BUTTON_1,      NES_MAP_B        },
    { "A",      WPAD_BUTTON_A,      NES_MAP_NONE     },
    { "B",      WPAD_BUTTON_B,      NES_MAP_OEKA_BTN },
    { NULL,     0,                  NES_MAP_NONE     },
    { NULL,     0,                  NES_MAP_NONE     },
    { NULL,     0,                  NES_MAP_NONE     },
    { NULL,     0,                  NES_MAP_NONE     }
  },
  DEFAULT_NUNCHUK_CONTROLS,
  DEFAULT_CLASSIC_CONTROLS,
  DEFAULT_GAMECUBE_CONTROLS
};

const WiiButton NesDbManager::
  WII_HYPERSHOT_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT] =
{
  {
    { "Plus",   WPAD_BUTTON_PLUS,   NES_MAP_START    },
    { "Minus",  WPAD_BUTTON_MINUS,  NES_MAP_SELECT   },
    { "2",      WPAD_BUTTON_2,      NES_MAP_HS_RUN   }, 
    { "1",      WPAD_BUTTON_1,      NES_MAP_HS_RUN   },
    { "A",      WPAD_BUTTON_A,      NES_MAP_HS_JUMP  },
    { "B",      WPAD_BUTTON_B,      NES_MAP_NONE     },
    { NULL,     0,                  NES_MAP_NONE     },
    { NULL,     0,                  NES_MAP_NONE     },
    { NULL,     0,                  NES_MAP_NONE     },
    { NULL,     0,                  NES_MAP_NONE     }
  },
    {                                                      \
    { "Plus",   WPAD_BUTTON_PLUS,       NES_MAP_START   }, \
    { "Minus",  WPAD_BUTTON_MINUS,      NES_MAP_SELECT  }, \
    { "2",      WPAD_BUTTON_2,          NES_MAP_SPECIAL }, \
    { "1",      WPAD_BUTTON_1,          NES_MAP_SPECIAL }, \
    { "A",      WPAD_BUTTON_A,          NES_MAP_HS_RUN  }, \
    { "B",      WPAD_BUTTON_B,          NES_MAP_HS_RUN  }, \
    { "C",      WPAD_NUNCHUK_BUTTON_C,  NES_MAP_B       }, \
    { "Z",      WPAD_NUNCHUK_BUTTON_Z,  NES_MAP_NONE    }, \
    { NULL,     0,                      NES_MAP_NONE    }, \
    { NULL,     0,                      NES_MAP_NONE    }  \
  },
  {                                                              \
    { "Plus",   WPAD_CLASSIC_BUTTON_PLUS,     NES_MAP_START   }, \
    { "Minus",  WPAD_CLASSIC_BUTTON_MINUS,    NES_MAP_SELECT  }, \
    { "A",      WPAD_CLASSIC_BUTTON_A,        NES_MAP_HS_RUN  }, \
    { "B",      WPAD_CLASSIC_BUTTON_B,        NES_MAP_HS_RUN  }, \
    { "X",      WPAD_CLASSIC_BUTTON_X,        NES_MAP_NONE    }, \
    { "Y",      WPAD_CLASSIC_BUTTON_Y,        NES_MAP_NONE    }, \
    { "R",      WPAD_CLASSIC_BUTTON_FULL_R,   NES_MAP_SPECIAL }, \
    { "L",      WPAD_CLASSIC_BUTTON_FULL_L,   NES_MAP_SPECIAL }, \
    { "zR",     WPAD_CLASSIC_BUTTON_ZR,       NES_MAP_SPECIAL }, \
    { "zL",     WPAD_CLASSIC_BUTTON_ZL,       NES_MAP_SPECIAL }  \
  },
  {                                                              \
    { "Start",  PAD_BUTTON_START,             NES_MAP_START   }, \
    { "A",      PAD_BUTTON_A,                 NES_MAP_HS_RUN  }, \
    { "B",      PAD_BUTTON_B,                 NES_MAP_HS_RUN  }, \
    { "X",      PAD_BUTTON_X,                 NES_MAP_SELECT  }, \
    { "Y",      PAD_BUTTON_Y,                 NES_MAP_SELECT  }, \
    { "R",      PAD_TRIGGER_R,                NES_MAP_SPECIAL }, \
    { "L",      PAD_TRIGGER_L,                NES_MAP_SPECIAL }, \
    { NULL,     0,                            NES_MAP_NONE    }, \
    { NULL,     0,                            NES_MAP_NONE    }, \
    { NULL,     0,                            NES_MAP_NONE    }  \
  }

};

static const char* profileNames[NES_PROFILE_COUNT] =
{
  "Gamepad", "Zapper", "Arkanoid Paddle", "Space Shadow Gun", "Oeka Kids Tablet", "Hypershot"
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

  entry->wiimoteMode = NES_WM_MODE_DEFAULT;
  entry->centerAdjust = NES_CENTER_ADJ_DEFAULT;
  entry->sensitivity = NES_SENSITIVITY_DEFAULT;
  entry->smoothing = NES_SMOOTHING_DEFAULT;
  setControls( NES_CONTROL_GAMEPADS );

  for( int i = 0; i < CurGame->DesiredInput.size(); i++ )
  {
    const char* input =  CurGame->DesiredInput[i];
    if( input != NULL )
    {
      if( !strcmp( input, "zapper" ) )
      {
        if( i == 0 )
        {
          setControls( NES_CONTROL_ZAPPER_P1 );
          break;
        }
        else if( i == 1 )
        {
          setControls( NES_CONTROL_ZAPPER_P2 );
          break;
        }
      }
      else if( !strcmp( input, "shadow" ) )
      {
        setControls( NES_CONTROL_SHADOW );
        break;
      }
      else if( !strcmp( input, "hypershot" ) )
      {
        setControls( NES_CONTROL_HYPERSHOT );
        break;
      }
      else if( !strcmp( input, "oekakids" ) )
      {
        setControls( NES_CONTROL_OEKAKIDS );
        break;
      }
      else if( !strcmp( input, "arkanoid" ) )
      {
        if( i == 1 )
        {
          setControls( NES_CONTROL_ARKANOID_P2 );
          break;
        }
        else if( i == 4 )
        {
          setControls( NES_CONTROL_ARKANOID_P5 );
          break;
        }                
      }
    }
  }

  resetButtons();
  applyButtonMap();
}

extern bool NESIsVSUni;
extern MDFNGI *MDFNGameInfo;

int NesDbManager::getMappableButtonCount( int profile )
{
  switch( profile )
  {
    case NES_PROFILE_ZAPPER:
      return zapperButtonCount;
    case NES_PROFILE_ARKANOID:
      return arkanoidButtonCount;
    case NES_PROFILE_SPACE_SHADOW:
      return spaceShadowButtonCount;
    case NES_PROFILE_OEKAKIDS:
      return oekakidsButtonCount;
    case NES_PROFILE_HYPERSHOT:
      return hypershotButtonCount;
    default:
      return buttonCount;
  }
}

const MappableButton* NesDbManager::getMappableButton( int profile, int button )
{
  switch( profile )
  {
    case NES_PROFILE_ZAPPER:
      return &NES_ZAPPER_BUTTONS[button];
    case NES_PROFILE_ARKANOID:
      return &NES_ARKANOID_BUTTONS[button];
    case NES_PROFILE_SPACE_SHADOW:
      return &NES_SPACE_SHADOW_BUTTONS[button];
    case NES_PROFILE_OEKAKIDS:
      return &NES_OEKAKIDS_BUTTONS[button];
    case NES_PROFILE_HYPERSHOT:
      return &NES_HYPERSHOT_BUTTONS[button];
    default:
      return &NES_BUTTONS[button];
  }
}

const WiiButton* NesDbManager::getMappedButton(
  int profile, int controller, int button )
{
  switch( profile )
  {
    case NES_PROFILE_ZAPPER:
      return &(WII_ZAPPER_BUTTONS[controller][button]);
    case NES_PROFILE_ARKANOID:
      return &(WII_ARKANOID_BUTTONS[controller][button]);
    case NES_PROFILE_SPACE_SHADOW:
      return &(WII_SPACE_SHADOW_BUTTONS[controller][button]);
    case NES_PROFILE_OEKAKIDS:
      return &(WII_OEKAKIDS_BUTTONS[controller][button]);
    case NES_PROFILE_HYPERSHOT:
      return &(WII_HYPERSHOT_BUTTONS[controller][button]);
    default:
      return &(WII_BUTTONS[controller][button]);
  }
}

bool NesDbManager::writeEntryValues( 
  FILE* file, const char* hash, const dbEntry *entry )
{
  if( !StandardDatabaseManager::writeEntryValues( file, hash, entry ) )
  {
    return false;
  }

  NesDbEntry* nesEntry = (NesDbEntry*)entry;
  fprintf( file, "control=%d\n", nesEntry->control );
  fprintf( file, "wiimoteMode=%d\n", nesEntry->wiimoteMode );
  fprintf( file, "centerAdj=%d\n", nesEntry->centerAdjust );
  fprintf( file, "sensitivity=%d\n", nesEntry->sensitivity );
  fprintf( file, "smoothing=%d\n", nesEntry->smoothing );

  return true;
}

void NesDbManager::readEntryValue( 
  dbEntry *entry, const char* name, const char* value )
{
  StandardDatabaseManager::readEntryValue( entry, name, value );

  NesDbEntry* nesEntry = (NesDbEntry*)entry;
  if( !strcmp( name, "control" ) )
  {
    nesEntry->control = Util_sscandec( value );
  }          
  else if( !strcmp( name, "wiimoteMode" ) )
  {
    nesEntry->wiimoteMode = Util_sscandec( value );
  }          
  else if( !strcmp( name, "centerAdj" ) )
  {
    nesEntry->centerAdjust = Util_sscandec( value );
  }          
  else if( !strcmp( name, "sensitivity" ) )
  {
    nesEntry->sensitivity = Util_sscandec( value );
  }          
  else if( !strcmp( name, "smoothing" ) )
  {
    nesEntry->smoothing = Util_sscandec( value );
  }          
}

void NesDbManager::setControls( int type )
{
  NesDbEntry* entry = (NesDbEntry*)getEntry();
  entry->control = type;

  switch( type )
  {
    case NES_CONTROL_ZAPPER_P1:
    case NES_CONTROL_ZAPPER_P2:
      entry->base.profile = NES_PROFILE_ZAPPER;
      break;
    case NES_CONTROL_ARKANOID_P2:
    case NES_CONTROL_ARKANOID_P5:
      entry->base.profile = NES_PROFILE_ARKANOID;
      break;
    case NES_CONTROL_SHADOW:
      entry->base.profile = NES_PROFILE_SPACE_SHADOW;
      break;
    case NES_CONTROL_OEKAKIDS:
      entry->base.profile = NES_PROFILE_OEKAKIDS;
      break;
    case NES_CONTROL_HYPERSHOT:
      entry->base.profile = NES_PROFILE_HYPERSHOT;
      break;
    default:
      entry->base.profile = NES_PROFILE_GAMEPADS;
      break;
  }
} 

int NesDbManager::getControls()
{
  NesDbEntry* entry = (NesDbEntry*)getEntry();
  return entry->control;
} 

bool NesDbManager::isProfileAvailable( int profile )
{
  NesDbEntry* entry = (NesDbEntry*)getEntry();

  return
    ( profile == NES_PROFILE_GAMEPADS ||  
      profile == entry->base.profile );
}


int NesDbManager::getProfileCount()
{
  return NES_PROFILE_COUNT;
}

const char* NesDbManager::getProfileName( int profile )
{
  return profileNames[profile];
}

u32 NesDbManager::getDefaultRewindButton( 
  int profile, int controller )
{
  if( controller == WII_CONTROLLER_MOTE &&
      ( profile != NES_PROFILE_GAMEPADS &&
        profile != NES_PROFILE_HYPERSHOT ) )
  {
    return 0;
  } 

  return 
    StandardDatabaseManager::getDefaultRewindButton( 
      profile, controller );
}
