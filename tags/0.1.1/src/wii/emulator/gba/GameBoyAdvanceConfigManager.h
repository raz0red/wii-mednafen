#ifndef GAMEBOY_ADVANCE_CONFIG_MANAGER_H
#define GAMEBOY_ADVANCE_CONFIG_MANAGER_H

#include "ConfigManager.h"

class GameBoyAdvanceConfigManager : public ConfigManager
{
public:
  GameBoyAdvanceConfigManager( Emulator& emulator );
  void writeConfig( FILE *fp );
  void readConfigValue( const char *name, const char* value );
};

#endif