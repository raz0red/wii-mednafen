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
  fprintf( fp, "nes.wiimote_mode=%d\n", nes.getWiimoteMode() );
  fprintf( fp, "nes.center_adjust=%d\n", nes.getCenterAdjust() );
  fprintf( fp, "nes.sensitivity=%d\n", nes.getSensitivity() );
  fprintf( fp, "nes.smoothing=%d\n", nes.getSmoothing() );
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
  else if( strcmp( name, "nes.wiimote_mode" ) == 0 )
  {
    nes.setWiimoteMode( Util_sscandec( value ) );
  } 
  else if( strcmp( name, "nes.center_adjust" ) == 0 )
  {
    nes.setCenterAdjust( Util_sscandec( value ) );
  } 
  else if( strcmp( name, "nes.sensitivity" ) == 0 )
  {
    nes.setSensitivity( Util_sscandec( value ) );
  } 
  else if( strcmp( name, "nes.smoothing" ) == 0 )
  {
    nes.setSmoothing( Util_sscandec( value ) );
  } 
}
