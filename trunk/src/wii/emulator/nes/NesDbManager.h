#ifndef NES_DB_MANAGER_H
#define NES_DB_MANAGER_H

#include "StandardDatabaseManager.h"

// Control type
#define NES_CONTROL_GAMEPADS      0
#define NES_CONTROL_ZAPPER_P2     1
#define NES_CONTROL_ZAPPER_P1     2
#define NES_CONTROL_ARKANOID_P2   3
#define NES_CONTROL_ARKANOID_P5   4
#define NES_CONTROL_SHADOW        5
#define NES_CONTROL_OEKAKIDS      6
#define NES_CONTROL_HYPERSHOT     7
#define NES_CONTROL_COUNT         8

// Profile
#define NES_PROFILE_GAMEPADS      0
#define NES_PROFILE_ZAPPER        1
#define NES_PROFILE_ARKANOID      2
#define NES_PROFILE_SPACE_SHADOW  3
#define NES_PROFILE_OEKAKIDS      4
#define NES_PROFILE_HYPERSHOT     5
#define NES_PROFILE_COUNT         6

// Wiimote mode
#define NES_WM_MODE_DEFAULT 0
#define NES_WM_MODE_IR      1
#define NES_WM_MODE_ROLL    2    
#define NES_WM_MODE_TILT    3

#define NES_CENTER_ADJ_DEFAULT  -100
#define NES_MIN_CENTER_ADJ      -5
#define NES_MAX_CENTER_ADJ      5

#define NES_SENSITIVITY_DEFAULT -100
#define NES_MIN_SENSITIVITY     -5
#define NES_MAX_SENSITIVITY     5

#define NES_SMOOTHING_DEFAULT   -1

/*
 * NES database entry 
 */
typedef struct NesDbEntry
{
  StandardDbEntry base;
  int control;
  int wiimoteMode;
  int centerAdjust;
  int sensitivity;
  int smoothing;
} NesDbEntry;

class NesDbManager : public StandardDatabaseManager
{
private:
  NesDbEntry m_entry;

protected:
  bool writeEntryValues( FILE* file, const char* hash, const dbEntry *entry );
  void readEntryValue( dbEntry *entry, const char* name, const char* value );

public:
  static const MappableButton NES_BUTTONS[];
  static const MappableButton NES_ZAPPER_BUTTONS[];
  static const MappableButton NES_ARKANOID_BUTTONS[];
  static const MappableButton NES_SPACE_SHADOW_BUTTONS[];
  static const MappableButton NES_OEKAKIDS_BUTTONS[];
  static const MappableButton NES_HYPERSHOT_BUTTONS[];

  static const WiiButton 
    WII_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT];
  static const WiiButton 
    WII_ZAPPER_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT];
  static const WiiButton 
    WII_ARKANOID_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT];
  static const WiiButton 
    WII_SPACE_SHADOW_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT];
  static const WiiButton 
    WII_OEKAKIDS_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT];
  static const WiiButton 
    WII_HYPERSHOT_BUTTONS[WII_CONTROLLER_COUNT][WII_MAP_BUTTON_COUNT];

  NesDbManager( Emulator& emulator );
  const WiiButton* getMappedButton( int profile, int controller, int button );
  int getMappableButtonCount( int profile );
  const MappableButton* getMappableButton( int profile, int button );
  void resetToDefaults();
  dbEntry* getEntry();
  bool isProfileAvailable( int profile );
  int getProfileCount(); 
  const char* getProfileName( int profile );
  u32 getDefaultRewindButton( int profile, int controller );

  void setControls( int type );
  int getControls();
};

#endif
