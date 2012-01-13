#ifndef GAMEBOY_ADVANCE_H
#define GAMEBOY_ADVANCE_H

#include "Emulator.h"
#include "wii_util.h"

#include "GameBoyAdvanceDbManager.h"
#include "GameBoyAdvanceConfigManager.h"
#include "GameBoyAdvanceMenuManager.h"

// GameBoy Advance Keys
#define GBA_NONE   0x0000
#define GBA_A      0x0001
#define GBA_B      0x0002
#define GBA_START  0x0008
#define GBA_SELECT 0x0004
#define GBA_RIGHT  0x0010
#define GBA_LEFT   0x0020
#define GBA_UP     0x0040
#define GBA_DOWN   0x0080
#define GBA_R      0x0100
#define GBA_L      0x0200
#define GBA_A_R    0x0001 | BTN_RAPID
#define GBA_B_R    0x0002 | BTN_RAPID
#define GBA_REWIND BTN_REWIND

class GameBoyAdvance : public Emulator
{
private:
  GameBoyAdvanceConfigManager m_configManager;
  GameBoyAdvanceDbManager m_dbManager;
  GameBoyAdvanceMenuManager m_menuManager;  
  bool m_gbaBios;

protected:
  const ScreenSize* getDoubleStrikeScreenSize();

public:
  GameBoyAdvance();
  ConfigManager& getConfigManager();
  DatabaseManager& getDbManager();
  MenuManager& getMenuManager();
  void updateControls( bool isRapid );
  bool isRotationSupported();

  const ScreenSize* getDefaultScreenSizes();
  int getDefaultScreenSizesCount();

  void onPostLoad();
  bool updateDebugText( char* output, const char* defaultOutput, int len );
  u8 getBpp();

  void setGbaBiosEnabled( bool enabled );
  bool isGbaBiosEnabled();

  bool isRewindSupported();

  friend class GameBoyAdvanceConfigManager;
};

#endif