#ifndef PCE_FAST_H
#define PCE_FAST_H

#include "Emulator.h"
#include "wii_util.h"

#include "PCEFastDbManager.h"
#include "PCEFastConfigManager.h"
#include "PCEFastMenuManager.h"

// PCE Keys
#define PCE_NONE   0x0000
#define PCE_I      0x0001
#define PCE_II     0x0002
#define PCE_SELECT 0x0004
#define PCE_RUN    0x0008
#define PCE_RIGHT  0x0020
#define PCE_LEFT   0x0080
#define PCE_UP     0x0010
#define PCE_DOWN   0x0040
#define PCE_III    0x0100
#define PCE_IV     0x0200
#define PCE_V      0x0400
#define PCE_VI     0x0800
#define PCE_I_R    PCE_I  | BTN_RAPID
#define PCE_II_R   PCE_II | BTN_RAPID
#define PCE_REWIND BTN_REWIND

#if 0
 { "mode_select", "2/6 Mode Select", 6, IDIT_BUTTON, NULL }, // ???
#endif

class PCEFast : public Emulator
{
private:
  PCEFastConfigManager m_configManager;
  PCEFastDbManager m_dbManager;
  PCEFastMenuManager m_menuManager; 

protected:
  virtual const ScreenSize* getDoubleStrikeScreenSize();

public:
  PCEFast();
  ConfigManager& getConfigManager();
  DatabaseManager& getDbManager();
  MenuManager& getMenuManager();
  void updateControls( bool isRapid );
  bool isRotationSupported();
  u8 getBpp();

  const ScreenSize* getDefaultScreenSizes();
  int getDefaultScreenSizesCount();

  void onPostLoad();
  bool updateDebugText( char* output, const char* defaultOutput, int len );

  bool isRewindSupported();

  friend class PCEFastConfigManager;
};

#endif