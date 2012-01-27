#include "ConfigManager.h"
#include "wii_util.h"

ConfigManager::ConfigManager( Emulator& emulator ) : BaseManager( emulator )
{
}

void ConfigManager::writeConfig( FILE *fp )
{  
  Emulator& emu = getEmulator();
  char buff[512];

  snprintf( buff, sizeof(buff), "%s.screen_width", emu.getKey() );
  fprintf( fp, "%s=%d\n", buff, emu.getScreenSize()->w  );
  snprintf( buff, sizeof(buff), "%s.screen_height", emu.getKey() );
  fprintf( fp, "%s=%d\n", buff, emu.getScreenSize()->h  );
  if( emu.isRotationSupported() )
  {
    snprintf( buff, sizeof(buff), "%s.rotscreen_width", emu.getKey() );
    fprintf( fp, "%s=%d\n", buff, emu.getRotatedScreenSize()->w  );
    snprintf( buff, sizeof(buff), "%s.rotscreen_height", emu.getKey() );
    fprintf( fp, "%s=%d\n", buff, emu.getRotatedScreenSize()->h  );
  }
  snprintf( buff, sizeof(buff), "%s.frameskip", emu.getKey() );
  fprintf( fp, "%s=%d\n", buff, emu.getFrameSkip()  );
  snprintf( buff, sizeof(buff), "%s.doublestrike", emu.getKey() );
  fprintf( fp, "%s=%d\n", buff, emu.getDoubleStrikeMode() );
  snprintf( buff, sizeof(buff), "%s.volume", emu.getKey() );
  fprintf( fp, "%s=%d\n", buff, emu.getVolume() );
}

void ConfigManager::readConfigValue( const char *name, const char* value )
{
  Emulator& emu = getEmulator();
  char buff[512];

  snprintf( buff, sizeof(buff), "%s.screen_width", emu.getKey() );
  if( strcmp( name, buff ) == 0 )
  {
    emu.getScreenSize()->w = Util_sscandec( value );
  }
  snprintf( buff, sizeof(buff), "%s.screen_height", emu.getKey() );
  if( strcmp( name, buff ) == 0 )
  {
    emu.getScreenSize()->h = Util_sscandec( value );
  }
  if( emu.isRotationSupported() )
  {
    snprintf( buff, sizeof(buff), "%s.rotscreen_width", emu.getKey() );
    if( strcmp( name, buff ) == 0 )
    {
      emu.getRotatedScreenSize()->w = Util_sscandec( value );
    }
    snprintf( buff, sizeof(buff), "%s.rotscreen_height", emu.getKey() );
    if( strcmp( name, buff ) == 0 )
    {
      emu.getRotatedScreenSize()->h = Util_sscandec( value );
    }
  }
  snprintf( buff, sizeof(buff), "%s.frameskip", emu.getKey() );
  if( strcmp( name, buff ) == 0 )
  {
    emu.setFrameSkip( Util_sscandec( value ) );
  }
  snprintf( buff, sizeof(buff), "%s.doublestrike", emu.getKey() );
  if( strcmp( name, buff ) == 0 )
  {
    emu.setDoubleStrikeMode( Util_sscandec( value ) );
  }  
  snprintf( buff, sizeof(buff), "%s.volume", emu.getKey() );
  if( strcmp( name, buff ) == 0 )
  {
    emu.setVolume( Util_sscandec( value ) );
  }    
}
