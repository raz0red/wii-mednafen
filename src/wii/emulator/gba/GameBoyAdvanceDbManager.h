#ifndef GAMEBOY_ADVANCE_DB_MANAGER_H
#define GAMEBOY_ADVANCE_DB_MANAGER_H

#include "StandardDatabaseManager.h"

/*
 * GameBoy Advance database entry 
 */
typedef struct GameBoyAdvanceDbEntry
{
  StandardDbEntry base;
} GameBoyAdvanceDbEntry;

class GameBoyAdvanceDbManager : public StandardDatabaseManager
{
private:
  GameBoyAdvanceDbEntry m_entry;

public:
  static const MappableButton GBA_BUTTONS[];
  static const WiiButton 
    WII_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT];
  GameBoyAdvanceDbManager( Emulator& emulator );
  const WiiButton* getMappedButton( int profile, int controller, int button );
  int getMappableButtonCount( int profile );
  const MappableButton* getMappableButton( int profile, int button );
  void resetToDefaults();
  dbEntry* getEntry();
};

#endif