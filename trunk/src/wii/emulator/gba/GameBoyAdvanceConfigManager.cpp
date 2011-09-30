#include "GameBoyAdvance.h"

GameBoyAdvanceConfigManager::GameBoyAdvanceConfigManager( Emulator &emulator )
  : ConfigManager( emulator )
{
}

void GameBoyAdvanceConfigManager::writeConfig( FILE *fp )
{
  ConfigManager::writeConfig( fp );

  GameBoyAdvance &emu = (GameBoyAdvance&)getEmulator();
  fprintf( fp, "gba.gba_bios=%d\n", emu.isGbaBiosEnabled() );
}

void GameBoyAdvanceConfigManager::readConfigValue( 
  const char *name, const char* value )
{
  ConfigManager::readConfigValue( name, value );

  GameBoyAdvance &emu = (GameBoyAdvance&)getEmulator();
  if( strcmp( name, "gba.gba_bios" ) == 0 )
  {
    emu.setGbaBiosEnabled( Util_sscandec( value ) );
  }  
}
