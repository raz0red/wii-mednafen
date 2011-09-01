#include "MasterSystem.h"

MasterSystemConfigManager::MasterSystemConfigManager( Emulator &emulator ) :
  ConfigManager( emulator )
{
}

void MasterSystemConfigManager::writeConfig( FILE *fp )
{
  ConfigManager::writeConfig( fp );

  MasterSystem &emu = (MasterSystem&)getEmulator();
}

void MasterSystemConfigManager::readConfigValue( 
  const char *name, const char* value )
{
  ConfigManager::readConfigValue( name, value );

  MasterSystem &emu = (MasterSystem&)getEmulator();
}
