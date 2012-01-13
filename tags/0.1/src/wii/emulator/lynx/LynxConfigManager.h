#ifndef LYNX_CONFIG_MANAGER_H
#define LYNX_CONFIG_MANAGER_H

#include "ConfigManager.h"

class LynxConfigManager : public ConfigManager
{
public:
  LynxConfigManager( Emulator& emulator );
  void writeConfig( FILE *fp );
  void readConfigValue( const char *name, const char* value );
};

#endif