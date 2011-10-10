#include "Emulator.h"
#include "BaseManager.h"

BaseManager::BaseManager( Emulator& emulator ) : m_emulator( emulator )
{
}

Emulator& BaseManager::getEmulator()
{
  return m_emulator;
}