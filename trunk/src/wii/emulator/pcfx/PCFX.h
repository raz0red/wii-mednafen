#ifndef PCFX_H
#define PCFX_H

#include "Emulator.h"
#include "wii_util.h"

#include "PCFXDbManager.h"
#include "PCFXConfigManager.h"
#include "PCFXMenuManager.h"

// PCFX Keys
#define PCFX_NONE   0x0000
#define PCFX_I      0x0001
#define PCFX_II     0x0002
#define PCFX_III    0x0004
#define PCFX_IV     0x0008
#define PCFX_V      0x0010
#define PCFX_VI     0x0020
#define PCFX_SELECT 0x0040
#define PCFX_RUN    0x0080
#define PCFX_UP     0x0100
#define PCFX_RIGHT  0x0200
#define PCFX_DOWN   0x0400
#define PCFX_LEFT   0x0800

class PCFX : public Emulator
{
private:
  PCFXConfigManager m_configManager;
  PCFXDbManager m_dbManager;
  PCFXMenuManager m_menuManager;  

protected:
  virtual const ScreenSize* getDoubleStrikeScreenSize();

public:
  PCFX();
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

  friend class PCFXConfigManager;
};

#endif