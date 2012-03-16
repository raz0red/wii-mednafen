#ifndef GAMEBOY_DB_MANAGER_H
#define GAMEBOY_DB_MANAGER_H

#include "StandardDatabaseManager.h"

/*
 * GameBoy database entry 
 */
typedef struct GameBoyDbEntry
{
  StandardDbEntry base;
} GameBoyDbEntry;

class GameBoyDbManager : public StandardDatabaseManager
{
private:
  GameBoyDbEntry m_entry;

public:
  static const MappableButton GB_BUTTONS[];
  static const WiiButton 
    WII_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT];
  GameBoyDbManager( Emulator& emulator );
  const WiiButton* getMappedButton( int profile, int controller, int button );
  int getMappableButtonCount( int profile );
  const MappableButton* getMappableButton( int profile, int button );
  void resetToDefaults();
  dbEntry* getEntry();
};

#endif