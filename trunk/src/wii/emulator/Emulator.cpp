#include "main.h"
#include "Emulator.h"
#include "DatabaseManager.h"
#include "wii_mednafen_main.h"

Emulator::Emulator( const char* key, const char* name ) : 
  m_key( key ),
  m_name( name ),
  m_padData( 0 )
{
}

Rect* Emulator::getScreenSize()
{
  return &m_screenSize;
}

Rect* Emulator::getDefaultScreenSize()
{
  return &m_defaultScreenSize;
}

Rect* Emulator::getRotatedScreenSize()
{
  return &m_rotatedScreenSize;
}

Rect* Emulator::getDefaultRotatedScreenSize()
{
  return &m_defaultRotatedScreenSize;
}

Rect* Emulator::getEmulatorScreenSize()
{
  return &m_emulatorScreenSize;
}

const char* Emulator::getKey()
{
  return m_key;
}

const char* Emulator::getName()
{
  return m_name;
}

u16 Emulator::getPadData()
{
  return m_padData;
}

void Emulator::onPostLoad()
{
}

bool Emulator::onShowControlsScreen()
{
  dbEntry* entry = getDbManager().getEntry();
  if( entry->wiimoteSupported )
  {
    // Wiimote is supported
    return true;
  }

  return wii_mednafen_show_controls_screen();
}

void Emulator::onPreLoop()
{
}

bool Emulator::updateDebugText( 
  char* output, const char* defaultOutput, int len )
{
  return false;
}

bool Emulator::isRotationSupported()
{
  return false;
}

int Emulator::getRotation()
{
  return MDFN_ROTATE0;
}

u8 Emulator::getBpp()
{
  return 16;
}

