#ifndef NEO_GEO_POCKET_DB_MANAGER_H
#define NEO_GEO_POCKET_DB_MANAGER_H

#include "StandardDatabaseManager.h"

// Game language
#define NGP_LANG_JAPANESE   0
#define NGP_LANG_ENGLISH    1

/*
 * NeoGeoPocket database entry 
 */
typedef struct NeoGeoPocketDbEntry
{
  StandardDbEntry base;
} NeoGeoPocketDbEntry;

class NeoGeoPocketDbManager : public StandardDatabaseManager
{
private:
  NeoGeoPocketDbEntry m_entry;

public:
  static const MappableButton NGP_BUTTONS[];
  static const WiiButton 
    WII_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT];
  NeoGeoPocketDbManager( Emulator& emulator );
  const WiiButton* getMappedButton( int profile, int controller, int button );
  int getMappableButtonCount( int profile );
  const MappableButton* getMappableButton( int profile, int button );
  void resetToDefaults();
  dbEntry* getEntry();
};

#endif