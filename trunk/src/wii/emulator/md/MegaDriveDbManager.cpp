#include "main.h"
#include "MegaDrive.h"
#include "MegaDriveDbManager.h"
#include <wiiuse/wpad.h>

// MD Keys
#define MD_MAP_NONE   KEY_MAP_NONE
#define MD_MAP_A      1
#define MD_MAP_B      2
#define MD_MAP_C      3
#define MD_MAP_X      4
#define MD_MAP_Y      5
#define MD_MAP_Z      6
#define MD_MAP_START  7
#define MD_MAP_MODE   8
#define MD_MAP_LEFT   9
#define MD_MAP_RIGHT  10
#define MD_MAP_UP     11
#define MD_MAP_DOWN   12
#define MD_MAP_A_R    13
#define MD_MAP_B_R    14
#define MD_MAP_C_R    15
#define MD_MAP_X_R    16
#define MD_MAP_Y_R    17
#define MD_MAP_Z_R    18
#define MD_MAP_REWIND 19


const MappableButton MegaDriveDbManager::MD_BUTTONS[] = 
{
  { "(none)",   MD_NONE     },
  { "A",        MD_A        }, 
  { "B",        MD_B        }, 
  { "C",        MD_C        },
  { "X",        MD_X        },
  { "Y",        MD_Y        },
  { "Z",        MD_Z        },
  { "Start",    MD_START    },
  { "Mode",     MD_MODE     },
  { "Left",     MD_LEFT     },
  { "Right",    MD_RIGHT    },
  { "Up",       MD_UP       },
  { "Down",     MD_DOWN     },
  { "A",        MD_A_R      }, 
  { "B",        MD_B_R      }, 
  { "C",        MD_C_R      },
  { "X",        MD_X_R      },
  { "Y",        MD_Y_R      },
  { "Z",        MD_Z_R      },
#ifdef ENABLE_MD_REWIND    
  { "(rewind)", MD_REWIND   }
#endif
};

static const int buttonCount =  
  sizeof(MegaDriveDbManager::MD_BUTTONS)/sizeof(MappableButton);

const WiiButton MegaDriveDbManager::
  WII_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT] =
{
  {
    { "Plus",   WPAD_BUTTON_PLUS,   MD_MAP_START  },
    { "Minus",  WPAD_BUTTON_MINUS,  MD_MAP_MODE   },
    { "2",      WPAD_BUTTON_2,      MD_MAP_C      }, 
    { "1",      WPAD_BUTTON_1,      MD_MAP_B      },
    { "A",      WPAD_BUTTON_A,      MD_MAP_A      },
    { "B",      WPAD_BUTTON_B,      MD_MAP_NONE   },
    { NULL,     0,                  MD_MAP_NONE   },
    { NULL,     0,                  MD_MAP_NONE   },
    { NULL,     0,                  MD_MAP_NONE   },
    { NULL,     0,                  MD_MAP_NONE   }
  },
  {
    { "Plus",   WPAD_BUTTON_PLUS,       MD_MAP_START  },
    { "Minus",  WPAD_BUTTON_MINUS,      MD_MAP_MODE   },
    { "2",      WPAD_BUTTON_2,          MD_MAP_NONE   }, 
    { "1",      WPAD_BUTTON_1,          MD_MAP_NONE   },
    { "A",      WPAD_BUTTON_A,          MD_MAP_A      },
    { "B",      WPAD_BUTTON_B,          MD_MAP_B      },
    { "C",      WPAD_NUNCHUK_BUTTON_C,  MD_MAP_NONE   },
    { "Z",      WPAD_NUNCHUK_BUTTON_Z,  MD_MAP_C      },
    { NULL,     0,                      MD_MAP_NONE   },
    { NULL,     0,                      MD_MAP_NONE   }
  },
  {
    { "Plus",   WPAD_CLASSIC_BUTTON_PLUS,     MD_MAP_START  },
    { "Minus",  WPAD_CLASSIC_BUTTON_MINUS,    MD_MAP_MODE   },
    { "A",      WPAD_CLASSIC_BUTTON_A,        MD_MAP_C      }, 
    { "B",      WPAD_CLASSIC_BUTTON_B,        MD_MAP_B      },
    { "X",      WPAD_CLASSIC_BUTTON_X,        MD_MAP_Y      },
    { "Y",      WPAD_CLASSIC_BUTTON_Y,        MD_MAP_A      },
    { "R",      WPAD_CLASSIC_BUTTON_FULL_R,   MD_MAP_Z      },
    { "L",      WPAD_CLASSIC_BUTTON_FULL_L,   MD_MAP_X      },
    { "zR",     WPAD_CLASSIC_BUTTON_ZR,       MD_MAP_Z      },
    { "zL",     WPAD_CLASSIC_BUTTON_ZL,       MD_MAP_X      }
  },
  {
    { "Start",  PAD_BUTTON_START,             MD_MAP_START  },
    { "A",      PAD_BUTTON_A,                 MD_MAP_B      },
    { "B",      PAD_BUTTON_B,                 MD_MAP_A      }, 
    { "X",      PAD_BUTTON_X,                 MD_MAP_C      },
    { "Y",      PAD_BUTTON_Y,                 MD_MAP_Y      },
    { "R",      PAD_TRIGGER_R,                MD_MAP_Z      },
    { "L",      PAD_TRIGGER_L,                MD_MAP_X      },
    { NULL,     0,                            MD_MAP_NONE   },
    { NULL,     0,                            MD_MAP_NONE   },
    { NULL,     0,                            MD_MAP_NONE   },
  }
};

MegaDriveDbManager::MegaDriveDbManager( Emulator &emulator ) :
  StandardDatabaseManager( emulator )
{
}

dbEntry* MegaDriveDbManager::getEntry()
{
  return (dbEntry*)&m_entry;
}

void MegaDriveDbManager::resetToDefaults()
{
  MegaDrive& emu = (MegaDrive&)getEmulator();
  MegaDriveDbEntry* entry = (MegaDriveDbEntry*)getEntry();
  memset( entry, 0x0, sizeof( MegaDriveDbEntry ) );

  StandardDatabaseManager::resetToDefaults();

  resetButtons();
  applyButtonMap();
}

int MegaDriveDbManager::getMappableButtonCount( int profile )
{
  return buttonCount;
}

const MappableButton* MegaDriveDbManager::getMappableButton( int profile, int button )
{
  return &MD_BUTTONS[button];
}

const WiiButton* MegaDriveDbManager::getMappedButton(
  int profile, int controller, int button )
{
  return &(WII_BUTTONS[controller][button]);
}

bool MegaDriveDbManager::writeEntryValues( 
  FILE* file, const char* hash, const dbEntry *entry )
{
  if( !StandardDatabaseManager::writeEntryValues( file, hash, entry ) )
  {
    return false;
  }

  MegaDriveDbEntry* MDEntry = (MegaDriveDbEntry*)entry;
  for( int i = 0; i < 2; i++ )
  {
    fprintf( file, "controlType.%d=%d\n", i, MDEntry->controlType[i] );
  }

  return true;
}

void MegaDriveDbManager::readEntryValue( 
  dbEntry *entry, const char* name, const char* value )
{
  StandardDatabaseManager::readEntryValue( entry, name, value );

  MegaDriveDbEntry* MDEntry = (MegaDriveDbEntry*)entry;
  for( int i = 0; i < 2; i++ )
  {
    char ctype[64];
    snprintf( ctype, sizeof(ctype), "controlType.%d", i );
     if( !strcmp( name, ctype ) )
    {
      MDEntry->controlType[i] = Util_sscandec( value );
    }   
  }
}