#ifndef WONDERSWAN_DB_MANAGER_H
#define WONDERSWAN_DB_MANAGER_H

#include "StandardDatabaseManager.h"

#define WS_PROFILE_NORMAL       0
#define WS_PROFILE_ROTATED90    1
#define WS_PROFILE_COUNT        2

// Game language
#define WS_LANG_JAPANESE   0
#define WS_LANG_ENGLISH    1

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
  static const MappableButton WS_BUTTONS[];
  static const WiiButton 
    WII_BUTTONS[WS_PROFILE_COUNT][WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT];
  WonderSwanDbManager( Emulator& emulator );
  const WiiButton* getMappedButton( int profile, int controller, int button );
  int getMappableButtonCount( int profile );
  const MappableButton* getMappableButton( int profile, int button );
  void resetToDefaults();
  dbEntry* getEntry();
  int getProfileCount(); 
  const char* getProfileName( int profile );
};

#endif