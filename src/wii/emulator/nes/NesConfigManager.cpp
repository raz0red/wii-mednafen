#include "Nes.h"

NesConfigManager::NesConfigManager( Emulator &emulator )
  : ConfigManager( emulator )
{
}

void NesConfigManager::writeConfig( FILE *fp )
{
  ConfigManager::writeConfig( fp );

  Nes &nes = (Nes&)getEmulator();
  fprintf( fp, "nes.game_genie=%d\n", nes.isGameGenieEnabled() );
}

void NesConfigManager::readConfigValue( 
  const char *name, const char* value )
{
  ConfigManager::readConfigValue( name, value );

  Nes &nes = (Nes&)getEmulator();
  if( strcmp( name, "nes.game_genie" ) == 0 )
  {
    nes.setGameGenieEnabled( Util_sscandec( value ) );
  } 
}
