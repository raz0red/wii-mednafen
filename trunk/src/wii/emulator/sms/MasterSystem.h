#ifndef MASTERSYSTEM_H
#define MASTERSYSTEM_H

#include "Emulator.h"
#include "wii_util.h"

#include "MasterSystemDbManager.h"
#include "MasterSystemConfigManager.h"
#include "MasterSystemMenuManager.h"

// MasterSystem keys
#define SMS_NONE      0x0000
#define SMS_UP        0x0001
#define SMS_DOWN      0x0002
#define SMS_LEFT      0x0004
#define SMS_RIGHT     0x0008
#define SMS_FIRE1     0x0010
#define SMS_FIRE2     0x0020
#define SMS_PAUSE     0x0040
#define SMS_FIRE1_R   SMS_FIRE1 | BTN_RAPID
#define SMS_FIRE2_R   SMS_FIRE2 | BTN_RAPID

class MasterSystem : public Emulator
{
private:
  MasterSystemConfigManager m_configManager;
  MasterSystemDbManager m_dbManager;
  MasterSystemMenuManager m_menuManager;  
  int m_consoleRegion;

  static const char* regions[2];
  static const char* regionNames[2];

public:
  static const int regionCount;

  MasterSystem();
  ConfigManager& getConfigManager();
  DatabaseManager& getDbManager();
  MenuManager& getMenuManager();
  void updateControls( bool isRapid );
  bool isDoubleStrikeSupported();
  bool isRotationSupported();

  void setConsoleRegion( int region );
  int getConsoleRegion();
  const char* getConsoleRegionString();
  const char* getConsoleRegionName();

  void onPostLoad();
  bool updateDebugText( char* output, const char* defaultOutput, int len );

  friend class MasterSystemConfigManager;
};

#endif