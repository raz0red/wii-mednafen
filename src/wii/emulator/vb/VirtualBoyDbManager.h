#ifndef VIRTUALBOY_DB_MANAGER_H
#define VIRTUALBOY_DB_MANAGER_H

#include "StandardDatabaseManager.h"

// ROM patching
#define ROM_PATCH_DEFAULT    0
#define ROM_PATCH_ENABLED    1
#define ROM_PATCH_DISABLED   2

/*
 * VB database entry 
 */
typedef struct VbDbEntry
{
  StandardDbEntry base;
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
  static const MappableButton VB_BUTTONS[];
  static const WiiButton 
    WII_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT];

  VirtualBoyDbManager( Emulator& emulator );
  const WiiButton* getMappedButton( int profile, int controller, int button );
  int getMappableButtonCount( int profile );
  const MappableButton* getMappableButton( int profile, int button );
  void resetToDefaults();
  dbEntry* getEntry();
  bool isRomPatchingEnabled();
};

#endif