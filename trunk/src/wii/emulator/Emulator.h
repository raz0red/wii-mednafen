#ifndef EMULATOR_H
#define EMULATOR_H

#include <gctypes.h>
#include <stdio.h>
#include <string>
using namespace std;

// Mednafen externs
extern volatile int GameThreadRun;

class ConfigManager;
class DatabaseManager;
class MenuManager;

typedef struct Rect {
  u16 w;
  u16 h;
} Rect;

class Emulator
{
private:
  const char* m_key; 
  const char* m_name;

protected:
  u16 m_padData;
  Rect m_emulatorScreenSize;
  Rect m_screenSize;
  Rect m_defaultScreenSize;
  Rect m_rotatedScreenSize;
  Rect m_defaultRotatedScreenSize;

  Emulator( const char* key, const char* name );

public:
  const char* getKey();
  const char* getName();
  virtual ConfigManager& getConfigManager() = 0;
  virtual DatabaseManager& getDbManager() = 0;
  virtual MenuManager& getMenuManager() = 0;
  virtual void updateControls() = 0;
  virtual void onPostLoad();
  virtual bool onShowControlsScreen();
  virtual void onPreLoop();
  virtual bool updateDebugText( 
    char* output, const char* defaultOutput, int len );
  virtual bool isRotationSupported();
  virtual int getRotation();
  virtual u8 getBpp();
  Rect* getEmulatorScreenSize();
  Rect* getScreenSize();
  Rect* getDefaultScreenSize();  
  Rect* getRotatedScreenSize();
  Rect* getDefaultRotatedScreenSize();  
  u16 getPadData();
};

#endif