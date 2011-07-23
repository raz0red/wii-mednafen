#ifndef NES_DB_MANAGER_H
#define NES_DB_MANAGER_H

#include "StandardDatabaseManager.h"

#define NES_BUTTON_COUNT         9

/*
 * NES database entry 
 */
typedef struct NesDbEntry
{
  StandardDbEntry base;
} NesDbEntry;

class NesDbManager : public StandardDatabaseManager
{
private:
  NesDbEntry m_entry;

public:
  static const MappableButton NES_BUTTONS[NES_BUTTON_COUNT];
  static const WiiButton 
    WII_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT];
  NesDbManager( Emulator& emulator );
  const WiiButton* getMappedButton( int profile, int controller, int button );
  int getMappableButtonCount();
  const MappableButton* getMappableButton( int button );
  void resetToDefaults();
  dbEntry* getEntry();
};

#endif