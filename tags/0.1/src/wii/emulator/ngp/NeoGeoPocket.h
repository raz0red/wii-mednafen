#ifndef NEO_GEO_POCKET_H
#define NEO_GEO_POCKET_H

#include "Emulator.h"
#include "wii_util.h"

#include "NeoGeoPocketDbManager.h"
#include "NeoGeoPocketConfigManager.h"
#include "NeoGeoPocketMenuManager.h"

// NeoGeoPocket keys
#define NGP_NONE    0x0000
#define NGP_UP      0x0001
#define NGP_DOWN    0x0002
#define NGP_LEFT    0x0004
#define NGP_RIGHT   0x0008
#define NGP_A       0x0010
#define NGP_B       0x0020
#define NGP_OPTION  0x0040
#define NGP_A_R     NGP_A | BTN_RAPID
#define NGP_B_R     NGP_B | BTN_RAPID
#define NGP_REWIND  BTN_REWIND

class NeoGeoPocket : public Emulator
{
private:
  int m_gameLanguage;
  NeoGeoPocketConfigManager m_configManager;
  NeoGeoPocketDbManager m_dbManager;
  NeoGeoPocketMenuManager m_menuManager; 

protected:
  virtual const ScreenSize* getDoubleStrikeScreenSize();

public:
  NeoGeoPocket();
  ConfigManager& getConfigManager();
  DatabaseManager& getDbManager();
  MenuManager& getMenuManager();
  void updateControls( bool isRapid );
  bool isRotationSupported();

  const ScreenSize* getDefaultScreenSizes();
  int getDefaultScreenSizesCount();

  int getGameLanguage();
  void setGameLanguage( int lang );

  void onPostLoad();
  bool updateDebugText( char* output, const char* defaultOutput, int len );

  bool isRewindSupported();

  friend class NeoGeoPocketConfigManager;
};

#endif