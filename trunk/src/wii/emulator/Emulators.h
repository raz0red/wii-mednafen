#ifndef EMULATORS_H
#define EMULATORS_H

#include "Emulator.h"
#include "VirtualBoy.h"
#include "WonderSwan.h"

#include <map>
#include <string>
using namespace std;

class Emulators
{
private:
  map<string,Emulator*> m_emuMap;
  Emulator* m_currentEmulator;

public:
  VirtualBoy VirtualBoyEmu;
  WonderSwan WonderSwanEmu;

  Emulators();
  Emulator* getEmulator( const char* key );
  Emulator* setCurrentEmulator( const char* key );
  Emulator* getCurrentEmulator();
  void writeConfig( FILE *fp );
  void readConfigValue( const char *name, const char* value );
};

extern Emulators emuRegistry;

#endif