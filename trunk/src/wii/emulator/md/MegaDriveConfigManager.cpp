#include "MegaDriveConfigManager.h"

MegaDriveConfigManager::MegaDriveConfigManager( Emulator &emulator )
  : ConfigManager( emulator )
{
}

void MegaDriveConfigManager::writeConfig( FILE *fp )
{
  ConfigManager::writeConfig( fp );
}

void MegaDriveConfigManager::readConfigValue( 
  const char *name, const char* value )
{
  ConfigManager::readConfigValue( name, value );
}
