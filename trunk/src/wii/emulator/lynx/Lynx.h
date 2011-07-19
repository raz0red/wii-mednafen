#ifndef LYNX_H
#define LYNX_H

#include "Emulator.h"
#include "wii_util.h"

#include "LynxDbManager.h"
#include "LynxConfigManager.h"
#include "LynxMenuManager.h"

// Lynx keys
#define LYNX_NONE   0x0000
#define LYNX_A		  0x0001
#define LYNX_B		  0x0002
#define LYNX_OPT2		0x0004
#define LYNX_OPT1		0x0008
#define LYNX_LEFT		0x0010
#define LYNX_RIGHT	0x0020
#define LYNX_UP		  0x0040
#define LYNX_DOWN		0x0080
#define LYNX_PAUSE	0x0100

class Lynx : public Emulator
{
private:
  LynxConfigManager m_configManager;
  LynxDbManager m_dbManager;
  LynxMenuManager m_menuManager;  

public:
  Lynx();
  ConfigManager& getConfigManager();
  DatabaseManager& getDbManager();
  MenuManager& getMenuManager();
  void updateControls();
  bool isRotationSupported();
  int getRotation();

  void onPostLoad();
  bool updateDebugText( char* output, const char* defaultOutput, int len );

  friend class LynxConfigManager;
};

#endif