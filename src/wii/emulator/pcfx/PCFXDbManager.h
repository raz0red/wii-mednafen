
#ifndef PCFX_DB_MANAGER_H
#define PCFX_DB_MANAGER_H

#include "StandardDatabaseManager.h"

#define PCFX_BUTTON_COUNT         13

/*
 * PCE database entry 
 */
typedef struct PCFXDbEntry
{
  StandardDbEntry base;
} PCFXDbEntry;

class PCFXDbManager : public StandardDatabaseManager
{
private:
  PCFXDbEntry m_entry;

public:
  static const MappableButton PCFX_BUTTONS[PCFX_BUTTON_COUNT];
  static const WiiButton 
    WII_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT];
  PCFXDbManager( Emulator& emulator );
  const WiiButton* getMappedButton( int profile, int controller, int button );
  int getMappableButtonCount( int profile );
  const MappableButton* getMappableButton( int profile, int button );
  void resetToDefaults();
  dbEntry* getEntry();
};

#endif