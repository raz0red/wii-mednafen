#ifndef PCE_FAST_CONFIG_MANAGER_H
#define PCE_FAST_CONFIG_MANAGER_H

#include "ConfigManager.h"

class PCEFastConfigManager : public ConfigManager
{
public:
  PCEFastConfigManager( Emulator& emulator );
  void writeConfig( FILE *fp );
  void readConfigValue( const char *name, const char* value );
};

#endif