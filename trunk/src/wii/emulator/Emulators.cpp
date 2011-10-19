#include "Emulators.h"

Emulators emuRegistry;

Emulators::Emulators() :
  m_currentEmulator( NULL )
{
  m_emuMap[VirtualBoyEmu.getKey()] = &(VirtualBoyEmu);
  m_emuMap[WonderSwanEmu.getKey()] = &(WonderSwanEmu);
  m_emuMap[LynxEmu.getKey()] = &(LynxEmu);
  m_emuMap[GameBoyEmu.getKey()] = &(GameBoyEmu);
  m_emuMap[GameBoyAdvanceEmu.getKey()] = &(GameBoyAdvanceEmu);
  m_emuMap[NesEmu.getKey()] = &(NesEmu);
  m_emuMap[PCEFastEmu.getKey()] = &(PCEFastEmu);
  m_emuMap[PCFXEmu.getKey()] = &(PCFXEmu);
  m_emuMap[NeoGeoPocketEmu.getKey()] = &(NeoGeoPocketEmu);
  m_emuMap[GameGearEmu.getKey()] = &(GameGearEmu);
  m_emuMap[MasterSystemEmu.getKey()] = &(MasterSystemEmu);
  m_emuMap[MegaDriveEmu.getKey()] = &(MegaDriveEmu);
}

Emulator* Emulators::getEmulator( const char* key )
{
  map<string,Emulator*>::iterator iter = m_emuMap.find( key );
  if( iter != m_emuMap.end() )
  {
    return iter->second;
  }
  return NULL;
}

Emulator* Emulators::setCurrentEmulator( const char* key )
{
  m_currentEmulator = 
    ( key == NULL ? NULL : getEmulator( key ) );
  return m_currentEmulator;
}

Emulator* Emulators::getCurrentEmulator()
{
  return m_currentEmulator;
}

void Emulators::writeConfig( FILE *fp )
{
  for( map<string,Emulator*>::iterator iter = m_emuMap.begin(); 
    iter != m_emuMap.end(); iter++ )
  {
    iter->second->getConfigManager().writeConfig( fp );
  }
}

void Emulators::readConfigValue( const char *name, const char* value )
{
  for( map<string,Emulator*>::iterator iter = m_emuMap.begin();
    iter != m_emuMap.end(); iter++ )
  {
    iter->second->getConfigManager().readConfigValue( name, value );
  }
}
