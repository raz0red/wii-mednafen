#ifndef WONDERSWAN_H
#define WONDERSWAN_H

#include "Emulator.h"
#include "wii_util.h"

#include "WonderSwanDbManager.h"
#include "WonderSwanConfigManager.h"
#include "WonderSwanMenuManager.h"

// WonderSwan keys
#define WS_NONE   0x0000
#define WS_X1     0x0001
#define WS_X2     0x0002
#define WS_X3     0x0004
#define WS_X4     0x0008
#define WS_Y1     0x0010
#define WS_Y2     0x0020
#define WS_Y3     0x0040
#define WS_Y4     0x0080
#define WS_START  0x0100
#define WS_A      0x0200
#define WS_B      0x0400

  class WonderSwan : public Emulator
{
private:
  WonderSwanConfigManager m_configManager;
  WonderSwanDbManager m_dbManager;
  WonderSwanMenuManager m_menuManager;  

public:
  WonderSwan();
  ConfigManager& getConfigManager();
  DatabaseManager& getDbManager();
  MenuManager& getMenuManager();
  void updateControls();
  bool isRotationSupported();
  int getRotation();

  void onPostLoad();
  bool updateDebugText( char* output, const char* defaultOutput, int len );
  u8 getBpp();

  friend class WonderSwanConfigManager;
};

#endif