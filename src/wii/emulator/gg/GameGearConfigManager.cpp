#include "GameGear.h"

GameGearConfigManager::GameGearConfigManager( Emulator &emulator ) :
  ConfigManager( emulator )
{
}

void GameGearConfigManager::writeConfig( FILE *fp )
{
  ConfigManager::writeConfig( fp );

  GameGear &emu = (GameGear&)getEmulator();
}

void GameGearConfigManager::readConfigValue( 
  const char *name, const char* value )
{
  ConfigManager::readConfigValue( name, value );

  GameGear &emu = (GameGear&)getEmulator();
}
