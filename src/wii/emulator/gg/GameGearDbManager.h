#ifndef GAMEGEAR_DB_MANAGER_H
#define GAMEGEAR_DB_MANAGER_H

#include "StandardDatabaseManager.h"

/*
 * GameGear database entry 
 */
typedef struct GameGearDbEntry
{
  StandardDbEntry base;
} GameGearDbEntry;

class GameGearDbManager : public StandardDatabaseManager
{
private:
  GameGearDbEntry m_entry;

public:
  static const MappableButton GG_BUTTONS[];
  static const WiiButton 
    WII_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT];
  GameGearDbManager( Emulator& emulator );
  const WiiButton* getMappedButton( int profile, int controller, int button );
  int getMappableButtonCount( int profile );
  const MappableButton* getMappableButton( int profile, int button );
  void resetToDefaults();
  dbEntry* getEntry();
};

#endif