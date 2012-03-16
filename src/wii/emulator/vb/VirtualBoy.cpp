#include "main.h"

#include "ControlMacros.h"
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

  m_doubleStrike = DOUBLE_STRIKE_DISABLED;
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

extern bool DNeedRewind;
static bool specialheld = false;

void VirtualBoy::updateControls( bool isRapid )
{
  bool special = false;

  WPAD_ScanPads();
  PAD_ScanPads();

  int c = 0;
  READ_CONTROLS_STATE

  u16 result = 0;
  StandardDbEntry* entry = (StandardDbEntry*)getDbManager().getEntry();
  StandardDatabaseManager& dbManager = 
    (StandardDatabaseManager&)getDbManager();

  for( int i = 0; i < dbManager.getMappableButtonCount( entry->profile ); i++ )  
  {
    BEGIN_IF_BUTTON_HELD(entry->profile)
      u32 val = VirtualBoyDbManager::VB_BUTTONS[ i ].button;
      if( val == VB_KEY_REWIND )
      {
        special = true;
        if( !specialheld )
        {
          specialheld = true;
          DNeedRewind = true;
        }                    
      }
      else if( !( val & BTN_RAPID ) || isRapid )
      {
        result |= ( val & 0xFFFF );
      }   
    END_IF_BUTTON_HELD
  }    

  //
  // Left sticks and pads
  //

  IF_RIGHT
    result|=VB_L_RIGHT;
  IF_LEFT
    result|=VB_L_LEFT;
  IF_UP
    result|=VB_L_UP;
  IF_DOWN
    result|=VB_L_DOWN;  

  //
  // Right sticks and pads
  //

  IF_R_RIGHT
    result|=VB_R_RIGHT;
  IF_R_LEFT
    result|=VB_R_LEFT;
  IF_R_UP
    result|=VB_R_UP;
  IF_R_DOWN
    result|=VB_R_DOWN;

  m_padData[0] = result;

  if( !special )
  {
    specialheld = false;
    DNeedRewind = false;
  }
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

  specialheld = false;
  DNeedRewind = false;
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

bool VirtualBoy::isRewindSupported()
{
#ifdef ENABLE_VB_REWIND  
  return true;
#else
  return false;
#endif
}