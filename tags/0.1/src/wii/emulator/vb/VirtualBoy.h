#ifndef VIRTUALBOY_H
#define VIRTUALBOY_H

#include "Emulator.h"
#include "wii_util.h"

#include "VirtualBoyDbManager.h"
#include "VirtualBoyConfigManager.h"
#include "VirtualBoyMenuManager.h"

typedef struct Vb3dMode
{
  const char *key;
  const char *name;
  u32 lColor;
  u32 rColor;
  bool isParallax;
} Vb3dMode;

// Virtual boy keys
#define VB_NONE       0x0000
#define VB_KEY_A      0x0001
#define VB_KEY_B      0x0002
#define VB_KEY_R      0x0004
#define VB_KEY_L      0x0008
#define VB_R_UP       0x0010
#define VB_R_RIGHT    0x0020
#define VB_L_RIGHT    0x0040
#define VB_L_LEFT     0x0080
#define VB_L_DOWN     0x0100
#define VB_L_UP       0x0200
#define VB_KEY_START  0x0400
#define VB_KEY_SELECT 0x0800
#define VB_R_LEFT     0x1000
#define VB_R_DOWN     0x2000
#define VB_KEY_A_R    VB_KEY_A | BTN_RAPID
#define VB_KEY_B_R    VB_KEY_B | BTN_RAPID
#define VB_KEY_REWIND BTN_REWIND

class VirtualBoy : public Emulator
{
private:
  bool m_patchRom;
  RGBA m_customColors[2];
  bool m_customColorsParallax;
  char m_modeKey[255];
  bool m_currentRomPatched;
  VirtualBoyConfigManager m_configManager;
  VirtualBoyDbManager m_dbManager;
  VirtualBoyMenuManager m_menuManager;

protected:
  bool isDoubleStrikeSupported();

public:
  static const char* CUSTOM_MODE_KEY;
  static const char* DEFAULT_MODE_KEY;
  static const Vb3dMode VB_MODES[];
  static const int VB_MODE_COUNT;

  VirtualBoy();
  ConfigManager& getConfigManager();
  DatabaseManager& getDbManager();
  MenuManager& getMenuManager();
  void updateControls( bool isRapid );

  bool getPatchRom();
  void setPatchRom( bool patch );
  RGBA *getCustomColors();
  bool hasCustomColors();
  bool getCustomColorsParallax();
  bool isCustomMode( const Vb3dMode* mode );
  int getModeIndex( const char* key );
  int getModeIndex();
  void setMode( const char* key );
  Vb3dMode getMode();
  int getRenderRate();
  void onPostLoad();
  void onPreLoop();
  bool updateDebugText( char* output, const char* defaultOutput, int len );
  u8 getBpp();

  const ScreenSize* getDefaultScreenSizes();
  int getDefaultScreenSizesCount();

  bool isRewindSupported();

  friend class VirtualBoyConfigManager;
};

#endif