#include "MegaDriveConfigManager.h"
#include "MegaDrive.h"

MegaDriveConfigManager::MegaDriveConfigManager( Emulator &emulator )
  : ConfigManager( emulator )
{
}

void MegaDriveConfigManager::writeConfig( FILE *fp )
{
  ConfigManager::writeConfig( fp );

  MegaDrive &emu = (MegaDrive&)getEmulator();
  fprintf( fp, "md.region=%d\n", emu.getConsoleRegion() );
}

void MegaDriveConfigManager::readConfigValue( 
  const char *name, const char* value )
{
  ConfigManager::readConfigValue( name, value );

  MegaDrive &emu = (MegaDrive&)getEmulator();
  if( strcmp( name, "md.region" ) == 0 )
  {
    emu.setConsoleRegion( Util_sscandec( value ) );
  } 
}
