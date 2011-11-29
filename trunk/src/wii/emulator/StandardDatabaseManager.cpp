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
  for( int x = 0; x < getProfileCount(); x++ )
  {
    for( int i = 0; i < WII_CONTROLLER_COUNT; i++ )
    {
      for( int j = 0; j < WII_MAP_BUTTON_COUNT; j++ )
      {
        // TODO:MULTIPROF
        entry->buttonMap[x][i][j] = 
          getMappedButton( x, i, j )->defaultMapping; // TODO:MULTIPROF
      }
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
      u8 mappedButton = entry->buttonMap[entry->profile][i][j]; // TODO:MULTIPROF
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
  int profileCount = getProfileCount();
  for( int x = 0; x < profileCount; x++ )
  {
    for( i = 0; i < WII_CONTROLLER_COUNT; i++ )
    {
      for( int j = 0; j < WII_MAP_BUTTON_COUNT; j++ )
      {
        u8 val = entry->buttonMap[x][i][j]; // TODO:MULTIPROF
        if( val != getMappedButton( x, i, j )->defaultMapping )
        {
          if( profileCount > 1 )
          {
            fprintf( file, "btn.%d.%d.%d=%d\n", x, i, j, val );            
          }
          else
          {
            // Backward compatibility
            fprintf( file, "btn.%d.%d=%d\n", i, j, val );
          }
        }
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
    // This is a hack for now. Requires that profile be read prior to the 
    // button mappings... The other option is to write out all button 
    // mappings (don't skip the defaults).
    resetButtons(); 
  }

  int i;
  bool btnFound = false;
  int profileCount = getProfileCount();
  for( int x = 0; x < profileCount; x++ )
  {
    for( i = 0; !btnFound && i < WII_CONTROLLER_COUNT; i++ )
    {
      for( int j = 0; !btnFound && j < WII_MAP_BUTTON_COUNT; j++ )
      {
        char btnName[64];

        // Backward compatibility
        snprintf( btnName, sizeof(btnName), "btn.%d.%d", i, j );
        if( !strcmp( name, btnName ) )
        {
          entry->buttonMap[entry->profile][i][j] = Util_sscandec( value ); // TODO:MULTIPROF
          btnFound = true;
        }

        if( !btnFound )
        {
          snprintf( btnName, sizeof(btnName), "btn.%d.%d.%d", x, i, j );
          if( !strcmp( name, btnName ) )
          {
            entry->buttonMap[x][i][j] = Util_sscandec( value ); // TODO:MULTIPROF
            btnFound = true;
          }
        }
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