#ifndef NES_CONFIG_MANAGER_H
#define NES_CONFIG_MANAGER_H

#include "ConfigManager.h"

class NesConfigManager : public ConfigManager
{
public:
  NesConfigManager( Emulator& emulator );
  void writeConfig( FILE *fp );
  void readConfigValue( const char *name, const char* value );
};

#endif