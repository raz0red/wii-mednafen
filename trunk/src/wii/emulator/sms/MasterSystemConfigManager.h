#ifndef MASTERSYSTEM_CONFIG_MANAGER_H
#define MASTERSYSTEM_CONFIG_MANAGER_H

#include "ConfigManager.h"

class MasterSystemConfigManager : public ConfigManager
{
public:
  MasterSystemConfigManager( Emulator& emulator );
  void writeConfig( FILE *fp );
  void readConfigValue( const char *name, const char* value );
};

#endif