#ifndef GAMEBOY_H
#define GAMEBOY_H

#include "Emulator.h"
#include "wii_util.h"

#include "GameBoyDbManager.h"
#include "GameBoyConfigManager.h"
#include "GameBoyMenuManager.h"

// GameBoy keys
#define GB_NONE   0x0000
#define GB_A      0x0001
#define GB_B      0x0002
#define GB_START  0x0008
#define GB_SELECT 0x0004
#define GB_LEFT   0x0020
#define GB_RIGHT  0x0010
#define GB_UP     0x0040
#define GB_DOWN   0x0080
#define GB_A_R    GB_A | BTN_RAPID
#define GB_B_R    GB_B | BTN_RAPID
#define GB_REWIND BTN_REWIND

class GameBoy : public Emulator
{
private:
  GameBoyConfigManager m_configManager;
  GameBoyDbManager m_dbManager;
  GameBoyMenuManager m_menuManager; 

protected:
  const ScreenSize* getDoubleStrikeScreenSize();

public:
  GameBoy();
  ConfigManager& getConfigManager();
  DatabaseManager& getDbManager();
  MenuManager& getMenuManager();
  void updateControls( bool isRapid );
  bool isRotationSupported();

  const ScreenSize* getDefaultScreenSizes();
  int getDefaultScreenSizesCount();

  void onPostLoad();
  bool updateDebugText( char* output, const char* defaultOutput, int len );

  bool isRewindSupported();

  friend class GameBoyConfigManager;
};

#endif