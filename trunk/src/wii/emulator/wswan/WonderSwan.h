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
#define WS_A_R    WS_A | BTN_RAPID
#define WS_B_R    WS_B | BTN_RAPID
#define WS_ROTATE 0x00010000
#define WS_REWIND BTN_REWIND

class WonderSwan : public Emulator
{
private:
  int m_gameLanguage;
  WonderSwanConfigManager m_configManager;
  WonderSwanDbManager m_dbManager;
  WonderSwanMenuManager m_menuManager;  

protected:
  virtual const ScreenSize* getDoubleStrikeScreenSize();
  virtual const ScreenSize* getDoubleStrikeRotatedScreenSize();

public:
  WonderSwan();
  ConfigManager& getConfigManager();
  DatabaseManager& getDbManager();
  MenuManager& getMenuManager();
  void updateControls( bool isRapid );
  bool isRotationSupported();
  int getRotation();

  int getGameLanguage();
  void setGameLanguage( int lang );

  const ScreenSize* getDefaultScreenSizes();
  int getDefaultScreenSizesCount();
  const ScreenSize* getDefaultRotatedScreenSizes();
  int getDefaultRotatedScreenSizesCount();

  void onPostLoad();
  bool updateDebugText( char* output, const char* defaultOutput, int len );
  u8 getBpp();

  bool isRewindSupported();

  friend class WonderSwanConfigManager;
};

#endif