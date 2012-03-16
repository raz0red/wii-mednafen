#include "VirtualBoy.h"
#include <wiiuse/wpad.h>

#define VBK_MAP_NONE    KEY_MAP_NONE
#define VBK_MAP_A       1
#define VBK_MAP_B       2
#define VBK_MAP_L       3
#define VBK_MAP_R       4
#define VBK_MAP_START   5
#define VBK_MAP_SELECT  6
#define VBK_MAP_LLEFT   7
#define VBK_MAP_LRIGHT  8
#define VBK_MAP_LUP     9
#define VBK_MAP_LDOWN   10
#define VBK_MAP_RLEFT   11
#define VBK_MAP_RRIGHT  12
#define VBK_MAP_RUP     13
#define VBK_MAP_RDOWN   14
#define VBK_MAP_A_R     15
#define VBK_MAP_B_R     16
#define VBK_MAP_REWIND  17

const MappableButton VirtualBoyDbManager::VB_BUTTONS[] = 
{
  { "(none)",   VB_NONE        },
  { "A",        VB_KEY_A       }, 
  { "B",        VB_KEY_B       }, 
  { "L",        VB_KEY_L       },
  { "R",        VB_KEY_R       },
  { "Start",    VB_KEY_START   },
  { "Select",   VB_KEY_SELECT  },
  { "L-Left",   VB_L_LEFT      },
  { "L-Right",  VB_L_RIGHT     },
  { "L-Up",     VB_L_UP        },
  { "L-Down",   VB_L_DOWN      },
  { "R-Left",   VB_R_LEFT      },
  { "R-Right",  VB_R_RIGHT     },
  { "R-Up",     VB_R_UP        },
  { "R-Down",   VB_R_DOWN      },
  { "A",        VB_KEY_A_R     }, 
  { "B",        VB_KEY_B_R     },
#ifdef ENABLE_VB_REWIND  
  { "(rewind)", VB_KEY_REWIND  } 
#endif
};

static const int buttonCount =  
  sizeof(VirtualBoyDbManager::VB_BUTTONS)/sizeof(MappableButton);

const WiiButton VirtualBoyDbManager::WII_BUTTONS
  [WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT] =
{
  {
    { "Plus",   WPAD_BUTTON_PLUS,   VBK_MAP_START  },
    { "Minus",  WPAD_BUTTON_MINUS,  VBK_MAP_SELECT },
    { "2",      WPAD_BUTTON_2,      VBK_MAP_A      }, 
    { "1",      WPAD_BUTTON_1,      VBK_MAP_B      },
    { "A",      WPAD_BUTTON_A,      VBK_MAP_R      },
    { "B",      WPAD_BUTTON_B,      VBK_MAP_L      },
    { NULL,     0,                  VBK_MAP_NONE   },
    { NULL,     0,                  VBK_MAP_NONE   },
    { NULL,     0,                  VBK_MAP_NONE   },
    { NULL,     0,                  VBK_MAP_NONE   }
  },
  {
    { "Plus",   WPAD_BUTTON_PLUS,       VBK_MAP_START  },
    { "Minus",  WPAD_BUTTON_MINUS,      VBK_MAP_SELECT },
    { "2",      WPAD_BUTTON_2,          VBK_MAP_NONE   }, 
    { "1",      WPAD_BUTTON_1,          VBK_MAP_NONE   },
    { "A",      WPAD_BUTTON_A,          VBK_MAP_A      },
    { "B",      WPAD_BUTTON_B,          VBK_MAP_R      },
    { "C",      WPAD_NUNCHUK_BUTTON_C,  VBK_MAP_B      },
    { "Z",      WPAD_NUNCHUK_BUTTON_Z,  VBK_MAP_L      },
    { NULL,     0,                      VBK_MAP_NONE   },
    { NULL,     0,                      VBK_MAP_NONE   }

  },
  {
    { "Plus",   WPAD_CLASSIC_BUTTON_PLUS,     VBK_MAP_START  },
    { "Minus",  WPAD_CLASSIC_BUTTON_MINUS,    VBK_MAP_SELECT },
    { "A",      WPAD_CLASSIC_BUTTON_A,        VBK_MAP_A      }, 
    { "B",      WPAD_CLASSIC_BUTTON_B,        VBK_MAP_B      },
    { "X",      WPAD_CLASSIC_BUTTON_X,        VBK_MAP_NONE   },
    { "Y",      WPAD_CLASSIC_BUTTON_Y,        VBK_MAP_NONE   },
    { "R",      WPAD_CLASSIC_BUTTON_FULL_R,   VBK_MAP_R      },
    { "L",      WPAD_CLASSIC_BUTTON_FULL_L,   VBK_MAP_L      },
    { "zR",     WPAD_CLASSIC_BUTTON_ZR,       VBK_MAP_R      },
    { "zL",     WPAD_CLASSIC_BUTTON_ZL,       VBK_MAP_L      }
  },
  {
    { "Start",  PAD_BUTTON_START,             VBK_MAP_START  },
    { "A",      PAD_BUTTON_A,                 VBK_MAP_A      },
    { "B",      PAD_BUTTON_B,                 VBK_MAP_B      }, 
    { "X",      PAD_BUTTON_X,                 VBK_MAP_SELECT },
    { "Y",      PAD_BUTTON_Y,                 VBK_MAP_SELECT },
    { "R",      PAD_TRIGGER_R,                VBK_MAP_R      },
    { "L",      PAD_TRIGGER_L,                VBK_MAP_L      },
    { NULL,     0,                            VBK_MAP_NONE   },
    { NULL,     0,                            VBK_MAP_NONE   },
    { NULL,     0,                            VBK_MAP_NONE   },
  }
};

VirtualBoyDbManager::VirtualBoyDbManager( Emulator &emulator ) :
  StandardDatabaseManager( emulator )
{
}

dbEntry* VirtualBoyDbManager::getEntry()
{
  return (dbEntry*)&m_entry;
}

void VirtualBoyDbManager::resetToDefaults()
{
  VbDbEntry* vbEntry = (VbDbEntry*)getEntry();
  memset( vbEntry, 0x0, sizeof( VbDbEntry ) );

  StandardDatabaseManager::resetToDefaults();

  vbEntry->romPatch = ROM_PATCH_DEFAULT;

  resetButtons();
  applyButtonMap();
}

int VirtualBoyDbManager::getMappableButtonCount( int profile )
{
  return buttonCount;
}

const MappableButton* VirtualBoyDbManager::getMappableButton( int profile, int button )
{
  return &VB_BUTTONS[button];
}

const WiiButton* VirtualBoyDbManager::getMappedButton(
  int profile, int controller, int button )
{
  return &(WII_BUTTONS[controller][button]);
}

bool VirtualBoyDbManager::writeEntryValues( 
  FILE* file, const char* hash, const dbEntry *entry )
{
  if( !StandardDatabaseManager::writeEntryValues( file, hash, entry ) )
  {
    return false;
  }

  VbDbEntry* vbEntry = (VbDbEntry*)entry;

  fprintf( file, "romPatch=%d\n", vbEntry->romPatch );

  return true;
}

void VirtualBoyDbManager::readEntryValue( 
  dbEntry *entry, const char* name, const char* value )
{
  StandardDatabaseManager::readEntryValue( entry, name, value );

  VbDbEntry* vbEntry = (VbDbEntry*)entry;

  if( !strcmp( name, "romPatch" ) )
  {
    vbEntry->romPatch = Util_sscandec( value );
  }
}

bool VirtualBoyDbManager::isRomPatchingEnabled()
{
  VbDbEntry* vbEntry = (VbDbEntry*)getEntry();

  return
    ( ( vbEntry->romPatch == ROM_PATCH_ENABLED ) ||
      ( ( vbEntry->romPatch == ROM_PATCH_DEFAULT ) &&
        ( ((VirtualBoy&)getEmulator()).getPatchRom() ) ) );
}
