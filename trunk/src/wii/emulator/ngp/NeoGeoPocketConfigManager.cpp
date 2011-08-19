#include "NeoGeoPocket.h"

NeoGeoPocketConfigManager::NeoGeoPocketConfigManager( Emulator &emulator ) :
  ConfigManager( emulator )
{
}

void NeoGeoPocketConfigManager::writeConfig( FILE *fp )
{
  ConfigManager::writeConfig( fp );

  NeoGeoPocket &emu = (NeoGeoPocket&)getEmulator();
  fprintf( fp, "ngp.language=%d\n", emu.getGameLanguage() );
}

void NeoGeoPocketConfigManager::readConfigValue( 
  const char *name, const char* value )
{
  ConfigManager::readConfigValue( name, value );

  NeoGeoPocket &emu = (NeoGeoPocket&)getEmulator();
  if( strcmp( name, "ngp.language" ) == 0 )
  {
    emu.setGameLanguage( Util_sscandec( value ) );
  }  
}
