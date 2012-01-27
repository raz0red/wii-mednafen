#include "Lynx.h"

LynxConfigManager::LynxConfigManager( Emulator &emulator ) :
  ConfigManager( emulator )
{
}

void LynxConfigManager::writeConfig( FILE *fp )
{
  ConfigManager::writeConfig( fp );

  Lynx &emu = (Lynx&)getEmulator();
}

void LynxConfigManager::readConfigValue( 
  const char *name, const char* value )
{
  ConfigManager::readConfigValue( name, value );

  Lynx &emu = (Lynx&)getEmulator();
}
