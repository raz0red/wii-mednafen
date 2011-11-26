#ifndef WONDERSWAN_DB_MANAGER_H
#define WONDERSWAN_DB_MANAGER_H

#include "StandardDatabaseManager.h"

#define WII_PROFILE_NORMAL      0
#define WII_PROFILE_ROTATED90   1

#define WS_BUTTON_COUNT         14
#define WS_PROFILE_COUNT       2

/*
 * Wonderswan database entry 
 */
typedef struct WswanDbEntry
{
  StandardDbEntry base;
} WswanDbEntry;

class WonderSwanDbManager : public StandardDatabaseManager
{
private:
  WswanDbEntry m_entry;

public:
  static const MappableButton WS_BUTTONS[WS_BUTTON_COUNT];
  static const WiiButton 
    WII_BUTTONS[WS_PROFILE_COUNT][WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT];
  WonderSwanDbManager( Emulator& emulator );
  const WiiButton* getMappedButton( int profile, int controller, int button );
  int getMappableButtonCount();
  const MappableButton* getMappableButton( int button );
  void resetToDefaults();
  dbEntry* getEntry();
};

#endif