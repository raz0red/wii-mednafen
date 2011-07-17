#include "StandardDatabaseManager.h"

const char* StandardDatabaseManager::WII_CONTROLLER_NAMES[
  WII_CONTROLLER_COUNT] =
{
  "Wiimote", "Wiimote + Nunchuk", "Classic", "GameCube"
};

StandardDatabaseManager::StandardDatabaseManager( Emulator &emulator ) :
  DatabaseManager( emulator )
{
}

void StandardDatabaseManager::resetButtons()
{
  StandardDbEntry* entry = (StandardDbEntry*)getEntry();
  memset( entry->buttonMap, 0x0, sizeof(entry->buttonMap) );

  // Set the default button map values
  for( int i = 0; i < WII_CONTROLLER_COUNT; i++ )
  {
    for( int j = 0; j < WII_MAP_BUTTON_COUNT; j++ )
    {
      entry->buttonMap[i][j] = 
        getMappedButton( entry->profile, i, j )->defaultMapping;
    }
  }
}

void StandardDatabaseManager::applyButtonMap()
{
  StandardDbEntry* entry = (StandardDbEntry*)getEntry();

  memset( entry->appliedButtonMap, 0x0, sizeof(entry->appliedButtonMap) );
  for( int i = 0; i < WII_CONTROLLER_COUNT; i++ )
  {
    for( int j = 0; j < WII_MAP_BUTTON_COUNT; j++ )
    {
      u8 mappedButton = entry->buttonMap[i][j];
      if( mappedButton != KEY_MAP_NONE )
      {
        entry->appliedButtonMap[i][mappedButton] |= 
          getMappedButton( entry->profile, i, j )->button;
      }
    }
  }
}

bool StandardDatabaseManager::writeEntryValues( 
  FILE* file, const char* hash, const dbEntry *ent )
{
  if( !ent ) return false;

  DatabaseManager::writeEntryValues( file, hash, ent );

  StandardDbEntry* entry = (StandardDbEntry*)ent;    
  fprintf( file, "profile=%d\n", entry->profile );

  int i;
  for( i = 0; i < WII_CONTROLLER_COUNT; i++ )
  {
    for( int j = 0; j < WII_MAP_BUTTON_COUNT; j++ )
    {
      u8 val = entry->buttonMap[i][j];
      if( val != getMappedButton( entry->profile, i, j )->defaultMapping )
      {
        fprintf( file, "btn.%d.%d=%d\n", i, j, val );
      }
    }
  }

  for( i = 1; i < getMappableButtonCount(); i++ )
  {
    char* desc = entry->buttonDesc[i];
    if( desc[0] != '\0' )
    {      
      fprintf( file, "btnDesc%d=%s\n", i, desc );
    }
  }

  return true;
}

void StandardDatabaseManager::readEntryValue( 
    dbEntry *ent, const char* name, const char* value )
{
  if( !ent ) return;

  DatabaseManager::readEntryValue( ent, name, value );

  StandardDbEntry* entry = (StandardDbEntry*)ent;   
  if( !strcmp( name, "profile" ) )
  {
    entry->profile = Util_sscandec( value );
  }

  int i;
  bool btnFound = false;
  for( i = 0; !btnFound && i < WII_CONTROLLER_COUNT; i++ )
  {
    for( int j = 0; !btnFound && j < WII_MAP_BUTTON_COUNT; j++ )
    {
      char btnName[64];
      snprintf( btnName, sizeof(btnName), "btn.%d.%d", i, j );
      if( !strcmp( name, btnName ) )
      {
        entry->buttonMap[i][j] = Util_sscandec( value );
        btnFound = true;
      }
    }
  }

  btnFound = false;
  for( i = 1; !btnFound && i < getMappableButtonCount(); i++ )
  {
    char button[255];
    snprintf( button, sizeof(button), "btnDesc%d", i );
    if( !strcmp( name, button ) )
    {
      Util_strlcpy( entry->buttonDesc[i], 
        value, sizeof(entry->buttonDesc[i]) );
      btnFound = true;
    }          
  }
}