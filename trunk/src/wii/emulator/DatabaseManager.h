#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include "Emulator.h"
#include "BaseManager.h"
#include "wii_main.h"

// Frame skipping
#define FRAME_SKIP_DEFAULT    0
#define FRAME_SKIP_ENABLED    1
#define FRAME_SKIP_DISABLED   2

/*
 * Structure for a Wii button 
 */
typedef struct WiiButton
{
  const char* name;   // The button name
  u32 button;         // The button value
  u8 defaultMapping;  // The default button mapping
} WiiButton;

/*
 * Structure for mappable button
 */
typedef struct MappableButton
{ 
  const char* name;   // The button name
  u32 button;   // The button value
} MappableButton;

typedef struct dbEntry
{
  char name[255];       // The name of the game
  u8 loaded;            // Whether the settings were loaded 
  u8 wiimoteSupported;  // Whether the Wiimote is supported
  u8 frameSkip;         // Whether to allow frame skipping
} dbEntry;


class DatabaseManager : public BaseManager
{
private:
  char m_dbFile[WII_MAX_PATH];
  char m_dbTmpFile[WII_MAX_PATH];
  char m_dbOldFile[WII_MAX_PATH];

  bool getHash( const char* source, char* dest );

protected:
  const char* getDbFile();
  const char* getDbTmpFile();
  const char* getDbOldFile();
  virtual bool writeEntryValues( 
    FILE* file, const char* hash, const dbEntry *entry );
  virtual void readEntryValue( 
    dbEntry *entry, const char* name, const char* value );

public:
  DatabaseManager( Emulator& emulator );  
  virtual void resetToDefaults();
  virtual dbEntry* getEntry() = 0;
  virtual void loadEntry( const char* hash );
  bool deleteEntry( const char* hash );
  bool writeEntry( const char* hash, bool del = false );
  virtual void applyButtonMap();
  virtual bool isProfileAvailable( int profile );
  virtual int getProfileCount(); 
  virtual const char* getProfileName( int profile );
  virtual void addRewindButtons();
};

#endif