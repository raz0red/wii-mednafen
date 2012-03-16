
#ifndef PCE_FAST_DB_MANAGER_H
#define PCE_FAST_DB_MANAGER_H

#include "StandardDatabaseManager.h"

// Control type
#define PCE_CONTROL_2BUTTON   0
#define PCE_CONTROL_6BUTTON   1

/*
 * PCE database entry 
 */
typedef struct PCEFastDbEntry
{
  StandardDbEntry base;
  u8 controlType[4];
} PCEFastDbEntry;

class PCEFastDbManager : public StandardDatabaseManager
{
private:
  PCEFastDbEntry m_entry;

protected:
  bool writeEntryValues( FILE* file, const char* hash, const dbEntry *entry );
  void readEntryValue( dbEntry *entry, const char* name, const char* value );

public:
  static const MappableButton PCE_BUTTONS[];
  static const WiiButton 
    WII_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT];
  PCEFastDbManager( Emulator& emulator );
  const WiiButton* getMappedButton( int profile, int controller, int button );
  int getMappableButtonCount( int profile );
  const MappableButton* getMappableButton( int profile, int button );
  void resetToDefaults();
  dbEntry* getEntry();
};

#endif