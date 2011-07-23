#include "GameBoyAdvance.h"

GameBoyAdvanceConfigManager::GameBoyAdvanceConfigManager( Emulator &emulator )
  : ConfigManager( emulator )
{
}

void GameBoyAdvanceConfigManager::writeConfig( FILE *fp )
{
  ConfigManager::writeConfig( fp );

  GameBoyAdvance &emu = (GameBoyAdvance&)getEmulator();
}

void GameBoyAdvanceConfigManager::readConfigValue( 
  const char *name, const char* value )
{
  ConfigManager::readConfigValue( name, value );

  GameBoyAdvance &emu = (GameBoyAdvance&)getEmulator();
}
