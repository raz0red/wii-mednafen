#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "Emulator.h"
#include "BaseManager.h"

class ConfigManager : public BaseManager
{
public:
  ConfigManager( Emulator& emulator );  
  virtual void writeConfig( FILE *fp );
  virtual void readConfigValue( const char *name, const char* value );
};

#endif