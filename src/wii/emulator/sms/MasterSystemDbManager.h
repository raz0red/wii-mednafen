#ifndef MASTERSYSTEM_DB_MANAGER_H
#define MASTERSYSTEM_DB_MANAGER_H

#include "StandardDatabaseManager.h"

/*
 * MasterSystem database entry 
 */
typedef struct MasterSystemDbEntry
{
  StandardDbEntry base;
} MasterSystemDbEntry;

class MasterSystemDbManager : public StandardDatabaseManager
{
private:
  MasterSystemDbEntry m_entry;

public:
  static const MappableButton SMS_BUTTONS[];
  static const WiiButton 
    WII_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT];
  MasterSystemDbManager( Emulator& emulator );
  const WiiButton* getMappedButton( int profile, int controller, int button );
  int getMappableButtonCount( int profile );
  const MappableButton* getMappableButton( int profile, int button );
  void resetToDefaults();
  dbEntry* getEntry();
};

#endif