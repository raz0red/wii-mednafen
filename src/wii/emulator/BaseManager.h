#ifndef BASE_MANAGER_H
#define BASE_MANAGER_H

#include "Emulator.h"

class BaseManager
{
protected:
  Emulator& m_emulator;
  BaseManager( Emulator& emulator );

public:
  Emulator& getEmulator();
};

#endif