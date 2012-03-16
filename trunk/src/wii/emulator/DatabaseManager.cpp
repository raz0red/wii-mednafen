#include "DatabaseManager.h"
#include "wii_app.h"

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"  
#endif

DatabaseManager::DatabaseManager( Emulator& emulator ) : 
  BaseManager( emulator )
{
  m_dbFile[0] = '\0';
  m_dbTmpFile[0] = '\0';
  m_dbOldFile[0] = '\0';
}

const char* DatabaseManager::getDbFile()
{
  if( m_dbFile[0] == '\0' )
  {
    snprintf( m_dbFile, WII_MAX_PATH, "%s%s%s.db",
      wii_get_fs_prefix(), WII_DB_DIR, getEmulator().getKey() );
  }

  return m_dbFile;
}

const char* DatabaseManager::getDbTmpFile()
{
  if( m_dbTmpFile[0] == '\0' )
  {
    snprintf( m_dbTmpFile, WII_MAX_PATH, "%s%s%s.db.tmp",
      wii_get_fs_prefix(), WII_DB_DIR, getEmulator().getKey() );
  }

  return m_dbTmpFile;
}

const char* DatabaseManager::getDbOldFile()
{
  if( m_dbOldFile[0] == '\0' )
  {
    snprintf( m_dbOldFile, WII_MAX_PATH, "%s%s%s.db.old",
      wii_get_fs_prefix(), WII_DB_DIR, getEmulator().getKey() );
  }

  return m_dbOldFile;
}

bool DatabaseManager::getHash( const char* source, char* dest )
{
  int dbHashLen = 0;    // The length of the hash
  char *endIdx;		      // End index of the hash  
  const char *startIdx; // Start index of the hash

  startIdx = source;
  if( *startIdx == '[' )
  {
    ++startIdx;
    endIdx = strrchr( startIdx, ']' );
    if( endIdx != 0 )
    {				  			
      dbHashLen = endIdx - startIdx;  
      strncpy( dest, startIdx, dbHashLen );		  
      dest[dbHashLen] = '\0';
      return true;
    }    
  }

  return false;
} 

bool DatabaseManager::writeEntryValues( 
  FILE* file, const char* hash, const dbEntry *entry )
{
  if( !entry ) return false;

  fprintf( file, "[%s]\n", hash );
  fprintf( file, "name=%s\n", entry->name );
  fprintf( file, "wiimoteSupported=%d\n", entry->wiimoteSupported );
  fprintf( file, "frameSkip=%d\n", entry->frameSkip );

  return true;
}

void DatabaseManager::readEntryValue( 
    dbEntry *entry, const char* name, const char* value )
{
  if( !entry ) return;

  if( !strcmp( name, "wiimoteSupported" ) )
  {
    entry->wiimoteSupported = Util_sscandec( value );
  }  
  else if( !strcmp( name, "frameSkip" ) )
  {
    entry->frameSkip = Util_sscandec( value );
  }  
}

void DatabaseManager::loadEntry( const char* hash )
{
  char buff[255];     // The buffer to use when reading the file    
  FILE* dbFile;       // The database file
  char dbHash[255];   // A hash found in the file we are reading from
  int readMode = 0;   // The current read mode 
  char* ptr;          // Pointer into the current entry value

#ifdef WII_NETTRACE
  net_print_string( NULL, 0, "%s\n", getDbFile() );
  net_print_string( NULL, 0, "%s\n", getDbTmpFile() );
  net_print_string( NULL, 0, "%s\n", getDbOldFile() );
#endif
  
  // Populate the entry with the defaults
  resetToDefaults();

  dbEntry* entry = getEntry();

  dbFile = fopen( getDbFile(), "r" );

  if( dbFile != 0 )
  {	
    while( fgets( buff, sizeof(buff), dbFile ) != 0 )
    {                
      if( readMode == 2 )
      {
        // We moved past the current record, exit.
        break;
      }
      
      if( readMode == 1 )
      {
        // Read from the matching database entry        
        ptr = strchr( buff, '=' );
        if( ptr )
        {
          *ptr++ = '\0';
          Util_trim( buff );
          Util_trim( ptr );
          
          if( !strcmp( buff, "name" ) )
          {
            Util_strlcpy( entry->name, ptr, sizeof(entry->name) );          
          }
          else
          {
            readEntryValue( entry, buff, ptr );
          }
        }                
      }
    
      // Search for the hash
      if( getHash( buff, dbHash ) && readMode < 2 )
      {        
        if( readMode || !strcmp( hash, dbHash ) )
        {
          entry->loaded = 1;
          readMode++;        
        }                
      }
    }

    fclose( dbFile );
  }
}

bool DatabaseManager::deleteEntry( const char* hash )
{
  return writeEntry( hash, true );
}

bool DatabaseManager::writeEntry( const char* hash, bool del )
{  
  char buff[255];		    // The buffer to use when reading the file  
  char dbHash[255];	  // A hash found in the file we are reading from
  int copyMode = 0;	  // The current copy mode 
  FILE* tmpFile = 0;	  // The temp file
  FILE* oldFile = 0;	  // The old file

  dbEntry* entry = del ? NULL : getEntry();

  // The database file
  FILE* dbFile = fopen( getDbFile(), "r" );

  // A database file doesn't exist, create a new one
  if( !dbFile )
  {
    dbFile = fopen( getDbFile(), "w" );
    if( !dbFile )
    {
      // Unable to create DB file
      return false;
    }

    // Write the entry
    writeEntryValues( dbFile, hash, entry );

    fclose( dbFile );
  }  
  else
  {
    //
    // A database exists, search for the appropriate hash while copying
    // its current contents to a temp file
    //

    // Open up the temp file
    tmpFile = fopen( getDbTmpFile(), "w" );
    if( !tmpFile )
    {
      fclose( dbFile );

      // Unable to create temp file
      return false;
    }

    //
    // Loop and copy	
    //

    while( fgets( buff, sizeof(buff), dbFile ) != 0 )
    {	  	            
      // Check if we found a hash
      if( copyMode < 2 && getHash( buff, dbHash ) )
      {
        if( copyMode )
        {
          copyMode++;
        }
        else if( !strcmp( hash, dbHash ) )
        {		  
          // We have matching hashes, write out the new entry
          writeEntryValues( tmpFile, hash, entry );
          copyMode++;
        }
      }

      if( copyMode != 1 )
      {	
        fprintf( tmpFile, "%s", buff );
      }
    }

    if( !copyMode )
    {
      // We didn't find the hash in the database, add it
      writeEntryValues( tmpFile, hash, entry );	
    }

    fclose( dbFile );
    fclose( tmpFile );  

    //
    // Make sure the temporary file exists
    // We do this due to the instability of the Wii SD card
    //
    tmpFile = fopen( getDbTmpFile(), "r" );
    if( !tmpFile )
    {      
      // Unable to find temp file
      return 0;
    }
    fclose( tmpFile );

    // Delete old file (if it exists)
    if( ( oldFile = fopen( getDbOldFile(), "r" ) ) != 0 )
    {
      fclose( oldFile );
      if( remove( getDbOldFile() ) != 0 )
      {
        return false;
      }
    }

    // Rename database file to old file
    if( rename( getDbFile(), getDbOldFile() ) != 0 )
    {
      return false;
    }

    // Rename temp file to database file	
    if( rename( getDbTmpFile(), getDbFile() ) != 0 )
    {
      return false;
    }
  }

  return true;
}

void DatabaseManager::resetToDefaults()
{
  dbEntry* entry = getEntry();
  entry->wiimoteSupported = 1;
  entry->frameSkip = FRAME_SKIP_DEFAULT;
}

bool DatabaseManager::isProfileAvailable( int profile )
{
  return true;
}

int DatabaseManager::getProfileCount()
{
  return 1;
}

const char* DatabaseManager::getProfileName( int profile )
{
  return "";
}

void DatabaseManager::applyButtonMap()
{
}

void DatabaseManager::addRewindButtons()
{
}