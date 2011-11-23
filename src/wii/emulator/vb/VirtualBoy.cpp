#include "main.h"

#include "VirtualBoy.h"
#include "wii_util.h"
#include "wii_input.h"
#include "wii_hw_buttons.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"

const char *VirtualBoy::CUSTOM_MODE_KEY = "custom";
const char *VirtualBoy::DEFAULT_MODE_KEY = "red_black";

const Vb3dMode VirtualBoy::VB_MODES[] = {
  { "red_black", "(2d) Red/black", 0xFF0000, 0x000000, false },
  { "white_black", "(2d) White/black", 0xFFFFFF, 0x000000, false },
  { "red_blue", "(3d) Red/blue", 0xFF0000, 0x0000FF, true },
  { "red_cyan", "(3d) Red/cyan", 0xFF0000, 0x00B7EB, true },
  { "red_ecyan", "(3d) Red/electric cyan", 0xFF0000, 0x00FFFF, true },
  { "red_green", "(3d) Red/green", 0xFF0000, 0x00FF00, true },
  { "green_red", "(3d) Green/red", 0x00FF00, 0xFF0000, true },
  { "yellow_blue", "(3d) Yellow/blue", 0xFFFF00, 0x0000FF, true },
  { VirtualBoy::CUSTOM_MODE_KEY, "(custom)", 0x0, 0x0, true }
};

const int VirtualBoy::VB_MODE_COUNT = 
  sizeof( VirtualBoy::VB_MODES ) / sizeof( Vb3dMode );


static const ScreenSize defaultScreenSizes[] = 
{
  { { 320,         240        },  "1x"          },
  { { 320*1.5,     240*1.5    },  "1.5x"        },
  { { 320*1.667,   240*1.667  },  "Full screen" },
  { { 320*1.667,   240*2.017  },  "Fill screen" }
};

static const int defaultScreenSizesCount =
  sizeof( defaultScreenSizes ) / sizeof(ScreenSize);

namespace MDFN_IEN_VB
{
  extern bool PatchROM(bool checkROM);
  extern void VIP_SetParallaxDisable(bool disabled);
  extern void VIP_SetAnaglyphColors(uint32 lcolor, uint32 rcolor);
  extern void VIP_Set3DMode(uint32 mode, bool reverse, uint32 prescale, uint32 sbs_separation);
  extern int vb_skip_sum;
}

VirtualBoy::VirtualBoy() : 
  Emulator( "vb", "Virtual Boy" ),
  m_configManager( *this ),
  m_dbManager( *this ),
  m_menuManager( *this ),
  m_patchRom( true ),
  m_customColorsParallax( true ),
  m_currentRomPatched( false )
{
  memset( m_customColors, sizeof( m_customColors ), 0x0 );
  setMode( DEFAULT_MODE_KEY );

  // The emulator screen size
  m_emulatorScreenSize.w = 384;
  m_emulatorScreenSize.h = 224;

  // Set user screen sizes
  int defaultIndex = 2;
  m_screenSize.w = defaultScreenSizes[defaultIndex].r.w; 
  m_screenSize.h = defaultScreenSizes[defaultIndex].r.h; 
}

ConfigManager& VirtualBoy::getConfigManager()
{
  return m_configManager;
}

DatabaseManager& VirtualBoy::getDbManager()
{
  return m_dbManager;
}

MenuManager& VirtualBoy::getMenuManager()
{
  return m_menuManager;
}

void VirtualBoy::setMode( const char* key )
{
  Util_strlcpy( m_modeKey, key, sizeof( m_modeKey ) );
}

bool VirtualBoy::getPatchRom()
{
  return m_patchRom;
}

void VirtualBoy::setPatchRom( bool patch )
{
  m_patchRom = patch;
}

RGBA* VirtualBoy::getCustomColors()
{
  return m_customColors;
}

bool VirtualBoy::hasCustomColors()
{
  return 
    Util_rgbatovalue( &m_customColors[0], FALSE ) != 0 ||
    Util_rgbatovalue( &m_customColors[1], FALSE ) != 0;
}

bool VirtualBoy::getCustomColorsParallax()
{
  return m_customColorsParallax;
}

bool VirtualBoy::isCustomMode( const Vb3dMode* mode )
{
  return !strcmp( mode->key, CUSTOM_MODE_KEY );
}

int VirtualBoy::getModeIndex( const char* key )
{
  for( int i = 0; i < VB_MODE_COUNT; i++ )
  {
    Vb3dMode mode = VB_MODES[i];
    if( !strcmp( mode.key, key ) )
    {
      return i; 
    }
  }

  return -1;
}

int VirtualBoy::getModeIndex()
{
  int index = getModeIndex( m_modeKey );
  if( index == -1 )
  {  
    index = getModeIndex( DEFAULT_MODE_KEY );
  }
  return index;
}

Vb3dMode VirtualBoy::getMode()
{
  return VB_MODES[getModeIndex()];
}

void VirtualBoy::updateControls( bool isRapid )
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

  for( int i = 0; i < VB_BUTTON_COUNT; i++ )
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
      u32 val = VirtualBoyDbManager::VB_BUTTONS[ i ].button;
      if( !( val & BTN_RAPID ) || isRapid )
      {
        result |= ( val & 0xFFFF );
      }   
    }
  }    

  //
  // Left sticks and pads
  //

  if( wii_digital_right( !isNunchuk, isClassic, heldLeft ) ||
      ( gcHeld & GC_BUTTON_RIGHT ) ||
      wii_analog_right( expX, gcX ) )
    result|=VB_L_RIGHT;

  if( wii_digital_left( !isNunchuk, isClassic, heldLeft ) || 
      ( gcHeld & GC_BUTTON_LEFT ) ||                       
      wii_analog_left( expX, gcX ) )
    result|=VB_L_LEFT;

  if( wii_digital_up( !isNunchuk, isClassic, heldLeft ) || 
      ( gcHeld & GC_BUTTON_UP ) ||
      wii_analog_up( expY, gcY ) )
    result|=VB_L_UP;

  if( wii_digital_down( !isNunchuk, isClassic, heldLeft ) ||
      ( gcHeld & GC_BUTTON_DOWN ) ||
      wii_analog_down( expY, gcY ) )
    result|=VB_L_DOWN;  

  //
  // Right sticks and pads
  //

  if( wii_digital_right( !isNunchuk, 0, heldRight ) || 
      wii_analog_right( expRX, gcRX ) )
    result|=VB_R_RIGHT;

  if( wii_digital_left( !isNunchuk, 0, heldRight ) || 
      wii_analog_left( expRX, gcRX ) )
    result|=VB_R_LEFT;

  if( wii_digital_up( !isNunchuk, 0, heldRight ) || 
      wii_analog_up( expRY, gcRY ) )
    result|=VB_R_UP;

  if( wii_digital_down( !isNunchuk, 0, heldRight ) ||  
      wii_analog_down( expRY, gcRY ) )
    result|=VB_R_DOWN;

  m_padData[0] = result;
}

void VirtualBoy::onPostLoad()
{
  // Is the current ROM patched?
  m_currentRomPatched =
    MDFN_IEN_VB::PatchROM(
      m_dbManager.isRomPatchingEnabled() );

  // Reset color choice if custom is selected and it shouldn't be allowed
  Vb3dMode mode = getMode();
  if( isCustomMode( &mode ) && !hasCustomColors() )
  {
    setMode( VirtualBoy::DEFAULT_MODE_KEY );
  }
}

void VirtualBoy::onPreLoop()
{
  const Vb3dMode mode = getMode();
  if( isCustomMode( &mode ) )
  {
    MDFN_IEN_VB::VIP_SetParallaxDisable( !getCustomColorsParallax() );
    MDFN_IEN_VB::VIP_SetAnaglyphColors( 
      Util_rgbatovalue( &(getCustomColors()[0]), FALSE ),      
      Util_rgbatovalue( &(getCustomColors()[1]), FALSE )
    );
  }
  else
  {
    MDFN_IEN_VB::VIP_SetParallaxDisable( !mode.isParallax );
    MDFN_IEN_VB::VIP_SetAnaglyphColors( mode.lColor, mode.rColor );
  }
}

bool VirtualBoy::updateDebugText( 
  char* output, const char* defaultOutput, int len )
{
  snprintf( output, len, "%s (p:%d)",
    defaultOutput, m_currentRomPatched ? 1 : 0 );

  return true;
}

u8 VirtualBoy::getBpp()
{
  return VB_BPP;
}

const ScreenSize* VirtualBoy::getDefaultScreenSizes()
{
  return defaultScreenSizes;
}

int VirtualBoy::getDefaultScreenSizesCount()
{
  return defaultScreenSizesCount;
}

bool VirtualBoy::isDoubleStrikeSupported()
{
  return false;
}
