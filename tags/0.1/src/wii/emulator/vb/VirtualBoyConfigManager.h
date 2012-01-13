#ifndef VIRTUALBOY_CONFIG_MANAGER_H
#define VIRTUALBOY_CONFIG_MANAGER_H

#include "ConfigManager.h"

class VirtualBoyConfigManager : public ConfigManager
{
public:
  VirtualBoyConfigManager( Emulator& emulator );
  void writeConfig( FILE *fp );
  void readConfigValue( const char *name, const char* value );
};

#endif