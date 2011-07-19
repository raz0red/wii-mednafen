#ifndef GAMEBOY_CONFIG_MANAGER_H
#define GAMEBOY_CONFIG_MANAGER_H

#include "ConfigManager.h"

class GameBoyConfigManager : public ConfigManager
{
public:
  GameBoyConfigManager( Emulator& emulator );
  void writeConfig( FILE *fp );
  void readConfigValue( const char *name, const char* value );
};

#endif