#include "MasterSystem.h"

MasterSystemConfigManager::MasterSystemConfigManager( Emulator &emulator ) :
  ConfigManager( emulator )
{
}

void MasterSystemConfigManager::writeConfig( FILE *fp )
{
  ConfigManager::writeConfig( fp );

  MasterSystem &emu = (MasterSystem&)getEmulator();
  fprintf( fp, "sms.region=%d\n", emu.getConsoleRegion() );
}

void MasterSystemConfigManager::readConfigValue( 
  const char *name, const char* value )
{
  ConfigManager::readConfigValue( name, value );

  MasterSystem &emu = (MasterSystem&)getEmulator();
  if( strcmp( name, "sms.region" ) == 0 )
  {
    emu.setConsoleRegion( Util_sscandec( value ) );
  }
}