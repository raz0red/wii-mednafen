#ifndef WONDERSWAN_CONFIG_MANAGER_H
#define WONDERSWAN_CONFIG_MANAGER_H

#include "ConfigManager.h"

class WonderSwanConfigManager : public ConfigManager
{
public:
  WonderSwanConfigManager( Emulator& emulator );
  void writeConfig( FILE *fp );
  void readConfigValue( const char *name, const char* value );
};

#endif