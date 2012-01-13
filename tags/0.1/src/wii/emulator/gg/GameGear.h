#ifndef GAMEGEAR_H
#define GAMEGEAR_H

#include "Emulator.h"
#include "wii_util.h"

#include "GameGearDbManager.h"
#include "GameGearConfigManager.h"
#include "GameGearMenuManager.h"

// GameGear keys
#define GG_NONE     0x0000
#define GG_UP       0x0001
#define GG_DOWN     0x0002
#define GG_LEFT     0x0004
#define GG_RIGHT    0x0008
#define GG_BTN1     0x0010
#define GG_BTN2     0x0020
#define GG_START    0x0040
#define GG_BTN1_R   GG_BTN1 | BTN_RAPID
#define GG_BTN2_R   GG_BTN2 | BTN_RAPID
#define GG_REWIND   BTN_REWIND

class GameGear : public Emulator
{
private:
  GameGearConfigManager m_configManager;
  GameGearDbManager m_dbManager;
  GameGearMenuManager m_menuManager;  

protected:
  const ScreenSize* getDoubleStrikeScreenSize();

public:
  GameGear();
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

  friend class GameGearConfigManager;
};

#endif