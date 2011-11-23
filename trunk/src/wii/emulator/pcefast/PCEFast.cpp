#include "main.h"

#include "PCEFast.h"
#include "wii_util.h"
#include "wii_input.h"
#include "wii_hw_buttons.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"

static const ScreenSize defaultScreenSizes[] = 
{
  { { 320,    240        },  "1x"             },
  { { 320*2,  240*2      },  "2x/Full screen" },
};

static const int defaultScreenSizesCount =
  sizeof( defaultScreenSizes ) / sizeof(ScreenSize);

PCEFast::PCEFast() : 
  Emulator( "pce_fast", "PC Engine" ),
  m_configManager( *this ),
  m_dbManager( *this ),
  m_menuManager( *this )
{
  // The emulator screen size
  m_emulatorScreenSize.w = 512;
  m_emulatorScreenSize.h = 242;

  // Set user screen sizes
  int defaultIndex = 1;
  m_screenSize.w = defaultScreenSizes[defaultIndex].r.w; 
  m_screenSize.h = defaultScreenSizes[defaultIndex].r.h; 
}

ConfigManager& PCEFast::getConfigManager()
{
  return m_configManager;
}

DatabaseManager& PCEFast::getDbManager()
{
  return m_dbManager;
}

MenuManager& PCEFast::getMenuManager()
{
  return m_menuManager;
}

// Mednafen extern
namespace PCE_Fast
{
  extern bool AVPad6Enabled[5];
}

void PCEFast::updateControls( bool isRapid )
{
  WPAD_ScanPads();
  PAD_ScanPads();

  for( int c = 0; c < 4; c++ )
  {
    // Check the state of the controllers
    u32 pressed = WPAD_ButtonsDown( c );
    u32 held = WPAD_ButtonsHeld( c );  
    u32 gcPressed = PAD_ButtonsDown( c );
    u32 gcHeld = PAD_ButtonsHeld( c );

    // Classic or Nunchuck?
    expansion_t exp;
    WPAD_Expansion( c, &exp );          

    BOOL isClassic = ( exp.type == WPAD_EXP_CLASSIC );
    BOOL isNunchuk = ( exp.type == WPAD_EXP_NUNCHUK );

    // Mask off the Wiimote d-pad depending on whether a nunchuk
    // is connected. (Wiimote d-pad is left when nunchuk is not
    // connected, right when it is).
    u32 heldLeft = ( isNunchuk ? ( held & ~0x0F00 ) : held );
    u32 heldRight = ( !isNunchuk ? ( held & ~0x0F00 ) : held );

    // Analog for Wii controls
    float expX = wii_exp_analog_val( &exp, TRUE, FALSE );
    float expY = wii_exp_analog_val( &exp, FALSE, FALSE );
    float expRX = isClassic ? wii_exp_analog_val( &exp, TRUE, TRUE ) : 0;
    float expRY = isClassic ? wii_exp_analog_val( &exp, FALSE, TRUE ) : 0;

    // Analog for Gamecube controls
    s8 gcX = PAD_StickX( c );
    s8 gcY = PAD_StickY( c );
    s8 gcRX = PAD_SubStickX( c );
    s8 gcRY = PAD_SubStickY( c );

    if( c == 0 )
    {
      // Check for home
      if( ( pressed & WII_BUTTON_HOME ) ||
        ( gcPressed & GC_BUTTON_HOME ) ||
        wii_hw_button )
      {
        GameThreadRun = 0;
      }
    }

    u16 result = 0;

    //
    // Mapped buttons
    //
    
    StandardDbEntry* entry = (StandardDbEntry*)getDbManager().getEntry();

    // 2 or 6 button controls
    PCE_Fast::AVPad6Enabled[c] = ((PCEFastDbEntry*)entry)->controlType[c];

    for( int i = 0; i < PCE_BUTTON_COUNT; i++ )
    {
      if( ( held &
            ( ( isClassic ? 
                  entry->appliedButtonMap[ 
                    WII_CONTROLLER_CLASSIC ][ i ] : 0 ) |
              ( isNunchuk ?
                  entry->appliedButtonMap[
                    WII_CONTROLLER_CHUK ][ i ] :
                  entry->appliedButtonMap[
                    WII_CONTROLLER_MOTE ][ i ] ) ) ) ||
          ( gcHeld &
              entry->appliedButtonMap[
                WII_CONTROLLER_CUBE ][ i ] ) )
      {
        u32 val = PCEFastDbManager::PCE_BUTTONS[ i ].button;
        if( !( val & BTN_RAPID ) || isRapid )
        {
          result |= ( val & 0xFFFF );
        }
      }
    }    

    if( wii_digital_right( !isNunchuk, isClassic, heldLeft ) ||
        ( gcHeld & GC_BUTTON_RIGHT ) ||
        wii_analog_right( expX, gcX ) )
      result|=PCE_RIGHT;

    if( wii_digital_left( !isNunchuk, isClassic, heldLeft ) || 
        ( gcHeld & GC_BUTTON_LEFT ) ||                       
        wii_analog_left( expX, gcX ) )
      result|=PCE_LEFT;

    if( wii_digital_up( !isNunchuk, isClassic, heldLeft ) || 
        ( gcHeld & GC_BUTTON_UP ) ||
        wii_analog_up( expY, gcY ) )
      result|=PCE_UP;

    if( wii_digital_down( !isNunchuk, isClassic, heldLeft ) ||
        ( gcHeld & GC_BUTTON_DOWN ) ||
        wii_analog_down( expY, gcY ) )
      result|=PCE_DOWN;

    m_padData[c] = result;
  }
}

void PCEFast::onPostLoad()
{
}

bool PCEFast::updateDebugText( 
  char* output, const char* defaultOutput, int len )
{
  return false;
}

bool PCEFast::isRotationSupported()
{
  return false;
}

u8 PCEFast::getBpp()
{
  return PCE_FAST_BPP;
}

const ScreenSize* PCEFast::getDefaultScreenSizes()
{ 
  return defaultScreenSizes;
}

int PCEFast::getDefaultScreenSizesCount()
{
  return defaultScreenSizesCount;
}

const ScreenSize* PCEFast::getDoubleStrikeScreenSize()
{
  return &defaultScreenSizes[1];
}