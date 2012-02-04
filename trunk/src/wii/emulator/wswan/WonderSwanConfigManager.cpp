#include "WonderSwan.h"

WonderSwanConfigManager::WonderSwanConfigManager( Emulator &emulator ) :
  ConfigManager( emulator )
{
}

void WonderSwanConfigManager::writeConfig( FILE *fp )
{
  ConfigManager::writeConfig( fp );

  WonderSwan &emu = (WonderSwan&)getEmulator();
  fprintf( fp, "wswan.language=%d\n", emu.getGameLanguage() );
}

void WonderSwanConfigManager::readConfigValue( 
  const char *name, const char* value )
{
  ConfigManager::readConfigValue( name, value );

  WonderSwan &emu = (WonderSwan&)getEmulator();
  if( strcmp( name, "wswan.language" ) == 0 )
  {
    emu.setGameLanguage( Util_sscandec( value ) );
  }  
}
