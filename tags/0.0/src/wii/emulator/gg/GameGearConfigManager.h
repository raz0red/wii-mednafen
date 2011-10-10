#ifndef GAMEGEAR_CONFIG_MANAGER_H
#define GAMEGEAR_CONFIG_MANAGER_H

#include "ConfigManager.h"

class GameGearConfigManager : public ConfigManager
{
public:
  GameGearConfigManager( Emulator& emulator );
  void writeConfig( FILE *fp );
  void readConfigValue( const char *name, const char* value );
};

#endif