#include "PCEFastConfigManager.h"

PCEFastConfigManager::PCEFastConfigManager( Emulator &emulator )
  : ConfigManager( emulator )
{
}

void PCEFastConfigManager::writeConfig( FILE *fp )
{
  ConfigManager::writeConfig( fp );
}

void PCEFastConfigManager::readConfigValue( 
  const char *name, const char* value )
{
  ConfigManager::readConfigValue( name, value );
}
