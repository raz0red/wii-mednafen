#ifndef STANDARD_DATABASE_MANAGER_H
#define STANDARD_DATABASE_MANAGER_H

#include "DatabaseManager.h"

#define KEY_MAP_NONE  0

#define WII_CONTROLLER_COUNT    4
#define WII_MAP_BUTTON_COUNT    10

#define WII_CONTROLLER_MOTE     0
#define WII_CONTROLLER_CHUK     1
#define WII_CONTROLLER_CLASSIC  2
#define WII_CONTROLLER_CUBE     3

#define WII_MAX_PROFILE_COUNT   10

/*
 * A standard, button-mapped database entry 
 */
typedef struct StandardDbEntry
{
  dbEntry base;
  u8 profile;  
  u8 buttonMap[WII_MAX_PROFILE_COUNT][WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT];
  u32 appliedButtonMap[WII_MAX_PROFILE_COUNT][WII_CONTROLLER_COUNT][32];
  char buttonDesc[WII_MAX_PROFILE_COUNT][32][255];
} StandardDbEntry;

class StandardDatabaseManager : public DatabaseManager
{
protected:
  virtual bool writeEntryValues( 
    FILE* file, const char* hash, const dbEntry *entry );
  virtual void readEntryValue( 
    dbEntry *entry, const char* name, const char* value );
  virtual u32 getDefaultRewindButton( int profile, int controller );

public:
  static const char* WII_CONTROLLER_NAMES[WII_CONTROLLER_COUNT];
  StandardDatabaseManager( Emulator& emulator );  
  virtual const WiiButton* getMappedButton( 
    int profile, int controller, int button ) = 0;  
  virtual int getMappableButtonCount( int profile ) = 0;
  virtual const MappableButton* getMappableButton( int profile, int button ) = 0;
  void resetButtons();
  virtual void applyButtonMap();
  virtual void addRewindButtons();
};

#endif