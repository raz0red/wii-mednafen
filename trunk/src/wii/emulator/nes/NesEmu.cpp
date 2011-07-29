#include "main.h"

#include "Nes.h"
#include "wii_util.h"
#include "wii_input.h"
#include "wii_hw_buttons.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"

Nes::Nes() : 
  Emulator( "nes", "NES" ),
  m_configManager( *this ),
  m_dbManager( *this ),
  m_menuManager( *this )
{
  // The emulator screen size
  m_emulatorScreenSize.w = 256;
  m_emulatorScreenSize.h = 256;

  // Set user screen sizes
  float hscale = 2.0;
  float wscale = 2.5;
  m_screenSize.w = m_defaultScreenSize.w = ((WII_WIDTH>>1)*wscale); 
  m_screenSize.h = m_defaultScreenSize.h = ((WII_HEIGHT>>1)*hscale);
}

ConfigManager& Nes::getConfigManager()
{
  return m_configManager;
}

DatabaseManager& Nes::getDbManager()
{
  return m_dbManager;
}

MenuManager& Nes::getMenuManager()
{
  return m_menuManager;
}

void Nes::updateControls()
{
  WPAD_ScanPads();
  PAD_ScanPads();

  // Check the state of the controllers
  u32 pressed = WPAD_ButtonsDown( 0 );
  u32 held = WPAD_ButtonsHeld( 0 );  
  u32 gcPressed = PAD_ButtonsDown( 0 );
  u32 gcHeld = PAD_ButtonsHeld( 0 );

  // Classic or Nunchuck?
  expansion_t exp;
  WPAD_Expansion( 0, &exp );          

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
  s8 gcX = PAD_StickX( 0 );
  s8 gcY = PAD_StickY( 0 );
  s8 gcRX = PAD_SubStickX( 0 );
  s8 gcRY = PAD_SubStickY( 0 );

  // Check for home
  if( ( pressed & WII_BUTTON_HOME ) ||
    ( gcPressed & GC_BUTTON_HOME ) ||
    wii_hw_button )
  {
    GameThreadRun = 0;
  }

  u16 result = 0;

  //
  // Mapped buttons
  //
  
  StandardDbEntry* entry = (StandardDbEntry*)getDbManager().getEntry();

  for( int i = 0; i < NES_BUTTON_COUNT; i++ )
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
      result |= NesDbManager::NES_BUTTONS[ i ].button;
    }
  }    

  if( wii_digital_right( !isNunchuk, isClassic, heldLeft ) ||
      ( gcHeld & GC_BUTTON_RIGHT ) ||
      wii_analog_right( expX, gcX ) )
    result|=NES_RIGHT;

  if( wii_digital_left( !isNunchuk, isClassic, heldLeft ) || 
      ( gcHeld & GC_BUTTON_LEFT ) ||                       
      wii_analog_left( expX, gcX ) )
    result|=NES_LEFT;

  if( wii_digital_up( !isNunchuk, isClassic, heldLeft ) || 
      ( gcHeld & GC_BUTTON_UP ) ||
      wii_analog_up( expY, gcY ) )
    result|=NES_UP;

  if( wii_digital_down( !isNunchuk, isClassic, heldLeft ) ||
      ( gcHeld & GC_BUTTON_DOWN ) ||
      wii_analog_down( expY, gcY ) )
    result|=NES_DOWN;

  m_padData = result;
}

void Nes::onPostLoad()
{
}

bool Nes::updateDebugText( 
  char* output, const char* defaultOutput, int len )
{
  return false;
}

bool Nes::isRotationSupported()
{
  return false;
}

u8 Nes::getBpp()
{
  return NES_BPP;
}