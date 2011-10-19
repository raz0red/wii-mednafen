#ifndef EMULATORS_H
#define EMULATORS_H

#include "Emulator.h"
#include "VirtualBoy.h"
#include "WonderSwan.h"
#include "Lynx.h"
#include "GameBoy.h"
#include "GameBoyAdvance.h"
#include "GameGear.h"
#include "PCEFast.h"
#include "PCFX.h"
#include "NeoGeoPocket.h"
#include "Nes.h"
#include "MasterSystem.h"
#include "MegaDrive.h"

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
  GameBoy GameBoyEmu;
  GameBoyAdvance GameBoyAdvanceEmu;
  Lynx LynxEmu;
  Nes NesEmu;
  PCEFast PCEFastEmu;
  PCFX PCFXEmu;
  NeoGeoPocket NeoGeoPocketEmu;
  GameGear GameGearEmu;
  MasterSystem MasterSystemEmu;
  MegaDrive MegaDriveEmu;

  Emulators();
  Emulator* getEmulator( const char* key );
  Emulator* setCurrentEmulator( const char* key );
  Emulator* getCurrentEmulator();
  void writeConfig( FILE *fp );
  void readConfigValue( const char *name, const char* value );
};

extern Emulators emuRegistry;

#endif