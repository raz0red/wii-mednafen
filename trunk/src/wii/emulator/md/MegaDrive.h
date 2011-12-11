#ifndef MEGADRIVE_H
#define MEGADRIVE_H

#include "Emulator.h"
#include "wii_util.h"

#include "MegaDriveDbManager.h"
#include "MegaDriveConfigManager.h"
#include "MegaDriveMenuManager.h"

// PCE Keys
#define MD_NONE   0x0000
#define MD_UP     0x0001
#define MD_DOWN   0x0002
#define MD_LEFT   0x0004
#define MD_RIGHT  0x0008
#define MD_B      0x0010
#define MD_C      0x0020
#define MD_A      0x0040
#define MD_START  0x0080
#define MD_Z      0x0100
#define MD_Y      0x0200
#define MD_X      0x0400
#define MD_MODE   0x0800
#define MD_A_R    MD_A | BTN_RAPID
#define MD_B_R    MD_B | BTN_RAPID
#define MD_C_R    MD_C | BTN_RAPID
#define MD_X_R    MD_X | BTN_RAPID
#define MD_Y_R    MD_Y | BTN_RAPID
#define MD_Z_R    MD_Z | BTN_RAPID
#define MD_REWIND BTN_REWIND

class MegaDrive : public Emulator
{
private:
  MegaDriveConfigManager m_configManager;
  MegaDriveDbManager m_dbManager;
  MegaDriveMenuManager m_menuManager; 
  int m_consoleRegion;

  static const char* regions[4];
  static const char* regionNames[4];

protected:
  virtual const ScreenSize* getDoubleStrikeScreenSize();

public:
  static const int regionCount;

  MegaDrive();
  ConfigManager& getConfigManager();
  DatabaseManager& getDbManager();
  MenuManager& getMenuManager();
  void updateControls( bool isRapid );
  bool isRotationSupported();
  u8 getBpp();

  void setConsoleRegion( int region );
  int getConsoleRegion();
  const char* getConsoleRegionString();
  const char* getConsoleRegionName();

  const ScreenSize* getDefaultScreenSizes();
  int getDefaultScreenSizesCount();
  
  void onPostLoad();
  void onPreLoop();
  bool updateDebugText( char* output, const char* defaultOutput, int len );

  bool isRewindSupported();

  friend class MegaDriveConfigManager;
};

#endif