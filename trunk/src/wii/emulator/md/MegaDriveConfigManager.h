#ifndef MEGADRIVE_CONFIG_MANAGER_H
#define MEGADRIVE_CONFIG_MANAGER_H

#include "ConfigManager.h"

class MegaDriveConfigManager : public ConfigManager
{
public:
  MegaDriveConfigManager( Emulator& emulator );
  void writeConfig( FILE *fp );
  void readConfigValue( const char *name, const char* value );
};

#endif