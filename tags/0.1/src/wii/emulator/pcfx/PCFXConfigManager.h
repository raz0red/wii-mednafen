#ifndef PCFX_CONFIG_MANAGER_H
#define PCFX_CONFIG_MANAGER_H

#include "ConfigManager.h"

class PCFXConfigManager : public ConfigManager
{
public:
  PCFXConfigManager( Emulator& emulator );
  void writeConfig( FILE *fp );
  void readConfigValue( const char *name, const char* value );
};

#endif