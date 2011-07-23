#include "Nes.h"

NesConfigManager::NesConfigManager( Emulator &emulator )
  : ConfigManager( emulator )
{
}

void NesConfigManager::writeConfig( FILE *fp )
{
  ConfigManager::writeConfig( fp );
}

void NesConfigManager::readConfigValue( 
  const char *name, const char* value )
{
  ConfigManager::readConfigValue( name, value );
}
