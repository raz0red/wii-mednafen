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

typedef struct ScreenSize {
  Rect r;
  const char* name;
} ScreenSize;

#define BTN_RAPID 0x80000000

class Emulator
{
private:
  const char* m_key; 
  const char* m_name;
  bool m_frameSkip;

protected:
  u16 m_padData[4];  
  Rect m_emulatorScreenSize;
  Rect m_screenSize;
  Rect m_rotatedScreenSize;

  Emulator( const char* key, const char* name );

  virtual bool isDoubleStrikeSupported();
  virtual const ScreenSize* getDoubleStrikeScreenSize();
  virtual const ScreenSize* getDoubleStrikeRotatedScreenSize();

public:
  const char* getKey();
  const char* getName();
  virtual ConfigManager& getConfigManager() = 0;
  virtual DatabaseManager& getDbManager() = 0;
  virtual MenuManager& getMenuManager() = 0;
  virtual void updateControls( bool isRapid ) = 0;
  virtual void onPostLoad();
  virtual bool onShowControlsScreen();
  virtual void onPreLoop();
  virtual bool updateDebugText( 
    char* output, const char* defaultOutput, int len );
  virtual bool isRotationSupported();
  virtual int getRotation();
  virtual u8 getBpp();
  virtual void resizeScreen();
  void getResizeScreenRect( Rect* rect );
  void getCurrentScreenSizeRatio( float* ratiox, float* ratioy );
 
  virtual const ScreenSize* getDefaultScreenSizes();
  virtual int getDefaultScreenSizesCount();
  const char* getScreenSizeName();
  const char* getScreenSizeName( int w, int h );

  virtual const ScreenSize* getDefaultRotatedScreenSizes();
  virtual int getDefaultRotatedScreenSizesCount();
  const char* getRotatedScreenSizeName();
  const char* getRotatedScreenSizeName( int w, int h );

  virtual bool isDoubleStrikeEnabled();

  Rect* getEmulatorScreenSize();
  Rect* getScreenSize();
  Rect* getRotatedScreenSize();

  u16* getPadData();
  bool getFrameSkip();
  void setFrameSkip( bool skip );
  bool getAppliedFrameSkip();
};

#endif