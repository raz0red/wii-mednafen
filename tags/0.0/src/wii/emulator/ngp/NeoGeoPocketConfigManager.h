#ifndef NEO_GEO_POCKET_CONFIG_MANAGER_H
#define NEO_GEO_POCKET_CONFIG_MANAGER_H

#include "ConfigManager.h"

class NeoGeoPocketConfigManager : public ConfigManager
{
public:
  NeoGeoPocketConfigManager( Emulator& emulator );
  void writeConfig( FILE *fp );
  void readConfigValue( const char *name, const char* value );
};

#endif