#include "PCFXConfigManager.h"

PCFXConfigManager::PCFXConfigManager( Emulator &emulator )
  : ConfigManager( emulator )
{
}

void PCFXConfigManager::writeConfig( FILE *fp )
{
  ConfigManager::writeConfig( fp );
}

void PCFXConfigManager::readConfigValue( 
  const char *name, const char* value )
{
  ConfigManager::readConfigValue( name, value );
}
