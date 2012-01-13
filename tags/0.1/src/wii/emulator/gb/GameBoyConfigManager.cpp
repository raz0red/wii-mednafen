#include "GameBoy.h"

GameBoyConfigManager::GameBoyConfigManager( Emulator &emulator ) :
  ConfigManager( emulator )
{
}

void GameBoyConfigManager::writeConfig( FILE *fp )
{
  ConfigManager::writeConfig( fp );

  GameBoy &emu = (GameBoy&)getEmulator();
}

void GameBoyConfigManager::readConfigValue( 
  const char *name, const char* value )
{
  ConfigManager::readConfigValue( name, value );

  GameBoy &emu = (GameBoy&)getEmulator();
}
