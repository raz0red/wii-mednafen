#include "VirtualBoy.h"

VirtualBoyConfigManager::VirtualBoyConfigManager( Emulator &emulator ) :
  ConfigManager( emulator )
{
}

void VirtualBoyConfigManager::writeConfig( FILE *fp )
{
  ConfigManager::writeConfig( fp );

  VirtualBoy &vbe = (VirtualBoy&)getEmulator();
  fprintf( fp, "vb.patch_rom=%d\n", vbe.m_patchRom );
  fprintf( fp, "vb.screen_mode=%s\n", vbe.m_modeKey );

  for( int i = 0; i < 2; i++ )
  {
    char hex[16];
    Util_rgbatohex( &vbe.m_customColors[i], hex );
    hex[6] = '\0'; // Ignore alpha
    fprintf( fp, ( !i ? "vb.custom.l=%s\n" : "vb.custom.r=%s\n"  ), hex );
  }
  fprintf( fp, "vb.custom.parallax=%d\n", vbe.m_customColorsParallax );
}

void VirtualBoyConfigManager::readConfigValue( 
  const char *name, const char* value )
{
  ConfigManager::readConfigValue( name, value );

  VirtualBoy &vbe = (VirtualBoy&)getEmulator();
  if( strcmp( name, "vb.patch_rom" ) == 0 )
  {
    vbe.m_patchRom = Util_sscandec( value ) != 0;
  }  
  else if( strcmp( name, "vb.custom.l" ) == 0 )
  {
    Util_hextorgba( value, &vbe.m_customColors[0] );
  }
  else if( strcmp( name, "vb.custom.r" ) == 0 )
  {
    Util_hextorgba( value, &vbe.m_customColors[1] );
  }
  else if( strcmp( name, "vb.custom.parallax" ) == 0 )
  {
    vbe.m_customColorsParallax = Util_sscandec( value ) != 0;
  }
  else if( strcmp( name, "vb.screen_mode" ) == 0 )
  {
    Util_strlcpy( vbe.m_modeKey, value, sizeof( vbe.m_modeKey ) );
  }
}
