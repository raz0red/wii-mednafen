#ifndef VIRTUALBOY_DB_MANAGER_H
#define VIRTUALBOY_DB_MANAGER_H

#include "StandardDatabaseManager.h"

// Render rate (frame skipping)
#define MAX_RENDER_RATE   99
#define MIN_RENDER_RATE   1

// ROM patching
#define ROM_PATCH_DEFAULT    0
#define ROM_PATCH_ENABLED    1
#define ROM_PATCH_DISABLED   2

#define VB_BUTTON_COUNT       15

/*
 * VB database entry 
 */
typedef struct VbDbEntry
{
  StandardDbEntry base;
  u8 frameSkip;         // Whether frame skipping is enabled
  u8 renderRate;        // The render rate (if skipping enabled)
  u8 romPatch;          // Whether to patch the ROM
} VbDbEntry;

class VirtualBoyDbManager : public StandardDatabaseManager
{
private:
  VbDbEntry m_entry;

protected:
  bool writeEntryValues( FILE* file, const char* hash, const dbEntry *entry );
  void readEntryValue( dbEntry *entry, const char* name, const char* value );

public:
  static const MappableButton VB_BUTTONS[VB_BUTTON_COUNT];
  static const WiiButton 
    WII_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT];

  VirtualBoyDbManager( Emulator& emulator );
  const WiiButton* getMappedButton( int profile, int controller, int button );
  int getMappableButtonCount();
  const MappableButton* getMappableButton( int button );
  void resetToDefaults();
  dbEntry* getEntry();
  bool isRomPatchingEnabled();
};

#endif