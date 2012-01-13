#ifndef NES_H
#define NES_H

#include "Emulator.h"
#include "wii_util.h"

#include "NesDbManager.h"
#include "NesConfigManager.h"
#include "NesMenuManager.h"

// NES Keys
#define NES_NONE    0x0000
#define NES_A       0x0001
#define NES_B       0x0002
#define NES_START   0x0008
#define NES_SELECT  0x0004
#define NES_RIGHT   0x0080
#define NES_LEFT    0x0040
#define NES_UP      0x0010
#define NES_DOWN    0x0020
#define NES_A_R     0x0001 | BTN_RAPID
#define NES_B_R     0x0002 | BTN_RAPID
#define NES_SPECIAL 0x00010000
#define NES_REWIND  BTN_REWIND

class Nes : public Emulator
{
private:
  bool m_gameGenie;
  NesConfigManager m_configManager;
  NesDbManager m_dbManager;
  NesMenuManager m_menuManager;  

protected:
  virtual const ScreenSize* getDoubleStrikeScreenSize();

public:
  Nes();
  ConfigManager& getConfigManager();
  DatabaseManager& getDbManager();
  MenuManager& getMenuManager();
  void updateControls( bool isRapid );
  bool isRotationSupported();
  u8 getBpp();  

  const ScreenSize* getDefaultScreenSizes();
  int getDefaultScreenSizesCount();

  bool isGameGenieEnabled();
  void setGameGenieEnabled( bool enabled );

  void onPostLoad();
  bool updateDebugText( char* output, const char* defaultOutput, int len );

  bool isRewindSupported();

  friend class NesConfigManager;
};

#endif