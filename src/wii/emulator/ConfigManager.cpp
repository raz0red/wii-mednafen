#include "ConfigManager.h"
#include "wii_util.h"

ConfigManager::ConfigManager( Emulator& emulator ) : BaseManager( emulator )
{
}

void ConfigManager::writeConfig( FILE *fp )
{  
  Emulator& emu = getEmulator();
  char buff[512];

  snprintf( buff, sizeof(buff), "%s.screen_w", emu.getKey() );
  fprintf( fp, "%s=%d\n", buff, emu.getScreenSize()->w  );
  snprintf( buff, sizeof(buff), "%s.screen_h", emu.getKey() );
  fprintf( fp, "%s=%d\n", buff, emu.getScreenSize()->h  );
  if( emu.isRotationSupported() )
  {
    snprintf( buff, sizeof(buff), "%s.rotscreen_w", emu.getKey() );
    fprintf( fp, "%s=%d\n", buff, emu.getRotatedScreenSize()->w  );
    snprintf( buff, sizeof(buff), "%s.rotscreen_h", emu.getKey() );
    fprintf( fp, "%s=%d\n", buff, emu.getRotatedScreenSize()->h  );
  }
}

void ConfigManager::readConfigValue( const char *name, const char* value )
{
  Emulator& emu = getEmulator();
  char buff[512];

  snprintf( buff, sizeof(buff), "%s.screen_w", emu.getKey() );
  if( strcmp( name, buff ) == 0 )
  {
    emu.getScreenSize()->w = Util_sscandec( value );
  }
  snprintf( buff, sizeof(buff), "%s.screen_h", emu.getKey() );
  if( strcmp( name, buff ) == 0 )
  {
    emu.getScreenSize()->h = Util_sscandec( value );
  }
  if( emu.isRotationSupported() )
  {
    snprintf( buff, sizeof(buff), "%s.rotscreen_w", emu.getKey() );
    if( strcmp( name, buff ) == 0 )
    {
      emu.getRotatedScreenSize()->w = Util_sscandec( value );
    }
    snprintf( buff, sizeof(buff), "%s.rotscreen_h", emu.getKey() );
    if( strcmp( name, buff ) == 0 )
    {
      emu.getRotatedScreenSize()->h = Util_sscandec( value );
    }
  }
}
