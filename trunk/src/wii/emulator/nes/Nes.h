#ifndef NES_H
#define NES_H

#include "Emulator.h"
#include "wii_util.h"

#include "NesDbManager.h"
#include "NesConfigManager.h"
#include "NesMenuManager.h"

// NES Keys
#define NES_NONE   0x0000
#define NES_A      0x0001
#define NES_B      0x0002
#define NES_START  0x0008
#define NES_SELECT 0x0004
#define NES_RIGHT  0x0080
#define NES_LEFT   0x0040
#define NES_UP     0x0010
#define NES_DOWN   0x0020
#define NES_A_R    0x0001 | BTN_RAPID
#define NES_B_R    0x0002 | BTN_RAPID

class Nes : public Emulator
{
private:
  NesConfigManager m_configManager;
  NesDbManager m_dbManager;
  NesMenuManager m_menuManager;  

public:
  Nes();
  ConfigManager& getConfigManager();
  DatabaseManager& getDbManager();
  MenuManager& getMenuManager();
  void updateControls( bool isRapid );
  bool isRotationSupported();
  u8 getBpp();

  void onPostLoad();
  bool updateDebugText( char* output, const char* defaultOutput, int len );

  friend class NesConfigManager;
};

#endif