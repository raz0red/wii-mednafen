#include "main.h"

#include "genio.h"

#include "MegaDrive.h"
#include "wii_util.h"
#include "wii_input.h"
#include "wii_hw_buttons.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"

const char* MegaDrive::regions[4] = 
{ 
   "game", 
   "overseas_ntsc", 
   "overseas_pal", 
   "domestic_ntsc"
};

const char* MegaDrive::regionNames[4] = 
{ 
   "Match game header", 
   "North America", 
   "Europe", 
   "Japan"
};

const int MegaDrive::regionCount = 
  sizeof( regions ) / sizeof( const char* );

MegaDrive::MegaDrive() : 
  Emulator( "md", "Mega Drive" ),
  m_configManager( *this ),
  m_dbManager( *this ),
  m_menuManager( *this ),
  m_consoleRegion( 0 )
{
  // The emulator screen size
  m_emulatorScreenSize.w = 320;
  m_emulatorScreenSize.h = 480;

  // Set user screen sizes
  float scalew = 2.2222;
  float scaleh = 2.0;
  m_screenSize.w = m_defaultScreenSize.w = ((WII_WIDTH>>1)*scalew); 
  m_screenSize.h = m_defaultScreenSize.h = ((WII_HEIGHT>>1)*scaleh);
}

ConfigManager& MegaDrive::getConfigManager()
{
  return m_configManager;
}

DatabaseManager& MegaDrive::getDbManager()
{
  return m_dbManager;
}

MenuManager& MegaDrive::getMenuManager()
{
  return m_menuManager;
}

void MegaDrive::updateControls( bool isRapid )
{
  WPAD_ScanPads();
  PAD_ScanPads();

  for( int c = 0; c < 2; c++ )
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

    for( int i = 0; i < MD_BUTTON_COUNT; i++ )
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
        u32 val = MegaDriveDbManager::MD_BUTTONS[ i ].button;
        if( !( val & BTN_RAPID ) || isRapid )
        {
          result |= ( val & 0xFFFF );
        }
      }
    }    

    if( wii_digital_right( !isNunchuk, isClassic, heldLeft ) ||
        ( gcHeld & GC_BUTTON_RIGHT ) ||
        wii_analog_right( expX, gcX ) )
      result|=MD_RIGHT;

    if( wii_digital_left( !isNunchuk, isClassic, heldLeft ) || 
        ( gcHeld & GC_BUTTON_LEFT ) ||                       
        wii_analog_left( expX, gcX ) )
      result|=MD_LEFT;

    if( wii_digital_up( !isNunchuk, isClassic, heldLeft ) || 
        ( gcHeld & GC_BUTTON_UP ) ||
        wii_analog_up( expY, gcY ) )
      result|=MD_UP;

    if( wii_digital_down( !isNunchuk, isClassic, heldLeft ) ||
        ( gcHeld & GC_BUTTON_DOWN ) ||
        wii_analog_down( expY, gcY ) )
      result|=MD_DOWN;

    m_padData[c] = result;
  }
}

void MegaDrive::onPostLoad()
{
}

void MegaDrive::onPreLoop()
{
  MegaDriveDbEntry* entry = (MegaDriveDbEntry*)getDbManager().getEntry();
  for( int c = 0; c < 2; c++ )
  {
    int ctype = entry->controlType[c];
    MDFN_IEN_MD::MDINPUT_SetInput(
      c, ( ctype == MD_CONTROL_3BUTTON ? "gamepad" : "gamepad6" ), NULL);
  }
}

bool MegaDrive::updateDebugText( 
  char* output, const char* defaultOutput, int len )
{
  return false;
}

bool MegaDrive::isRotationSupported()
{
  return false;
}

bool MegaDrive::isDoubleStrikeSupported()
{
  return true;
}

void MegaDrive::setConsoleRegion( int region )
{
  if( region >= 0 && region < regionCount )
  { 
    m_consoleRegion = region;
  }
}

int MegaDrive::getConsoleRegion()
{
  return m_consoleRegion;
}

const char* MegaDrive::getConsoleRegionString()
{
  return MegaDrive::regions[m_consoleRegion];
}

const char* MegaDrive::getConsoleRegionName()
{
  return MegaDrive::regionNames[m_consoleRegion];
}

u8 MegaDrive::getBpp()
{
  return MD_BPP;
}
