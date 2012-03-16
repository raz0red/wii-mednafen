
#ifndef MEGADRIVE_DB_MANAGER_H
#define MEGADRIVE_DB_MANAGER_H

#include "StandardDatabaseManager.h"

// Control type
#define MD_CONTROL_3BUTTON   0
#define MD_CONTROL_6BUTTON   1

/*
 * MD database entry 
 */
typedef struct MegaDriveDbEntry
{
  StandardDbEntry base;
  u8 controlType[4];
} MegaDriveDbEntry;

class MegaDriveDbManager : public StandardDatabaseManager
{
private:
  MegaDriveDbEntry m_entry;

protected:
  bool writeEntryValues( FILE* file, const char* hash, const dbEntry *entry );
  void readEntryValue( dbEntry *entry, const char* name, const char* value );

public:
  static const MappableButton MD_BUTTONS[];
  static const WiiButton 
    WII_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT];
  MegaDriveDbManager( Emulator& emulator );
  const WiiButton* getMappedButton( int profile, int controller, int button );
  int getMappableButtonCount( int profile );
  const MappableButton* getMappableButton( int profile, int button );
  void resetToDefaults();
  dbEntry* getEntry();
};

#endif