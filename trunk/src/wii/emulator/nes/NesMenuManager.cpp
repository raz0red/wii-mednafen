#include "main.h"

#include "NesMenuManager.h"
#include "NesDbManager.h"
#include "Nes.h"

#include "gettext.h"

#include "wii_app.h"
#include "wii_gx.h"
#include "wii_sdl.h"
#include "wii_main.h"
#include "wii_mednafen.h"
#include "wii_mednafen_main.h"

NesMenuManager::NesMenuManager( Emulator &emulator ) :
  MenuManager( emulator ),
  m_emuMenuHelper( emulator ),
  m_cartSettingsMenuHelper( emulator )
{
  //
  // The emulator menu
  // 
  m_emulatorMenu = m_emuMenuHelper.createEmulatorMenu( true );  

  TREENODE* emuControls = 
    wii_find_tree_node( m_emulatorMenu, NODETYPE_CONTROLS_SETTINGS );

  TREENODE* child = wii_create_tree_node( NODETYPE_EMU_WIIMOTE_MODE, "Arkanoid mode (Wiimote)" );
  wii_add_child( emuControls, child );  

  child = wii_create_tree_node( NODETYPE_EMU_CENTER_ADJUST, "Center adjust" );
  wii_add_child( emuControls, child );  

  child = wii_create_tree_node( NODETYPE_EMU_SENSITIVITY, "Sensitivity adjust" );
  wii_add_child( emuControls, child );  

  child = wii_create_tree_node( NODETYPE_EMU_SMOOTHING, "Smoothing" );
  wii_add_child( emuControls, child );  

  // Game Genie
  wii_add_child( m_emulatorMenu, 
    wii_create_tree_node( NODETYPE_GAME_GENIE, "Game Genie" ) );

  //
  // The cartridge settings (current) menu
  //
  m_cartridgeSettingsMenu = 
    m_cartSettingsMenuHelper.createCartridgeSettingsMenu();   

  // Controls sub-menu
  TREENODE *controls = 
    m_cartSettingsMenuHelper.addControlsSettingsNode( 
      m_cartridgeSettingsMenu );

  // Control mappings
  TREENODE *mappings = 
    m_cartSettingsMenuHelper.addControlsMappingsNode( 
      controls );

  m_cartSettingsMenuHelper.addProfileNode( mappings );
  m_cartSettingsMenuHelper.addControllerNode( mappings );
  m_cartSettingsMenuHelper.addWiimoteSupportedNode( mappings );
  m_cartSettingsMenuHelper.addButtonMappingNodes( mappings );

  m_cartSettingsMenuHelper.addSpacerNode( controls );

  child = wii_create_tree_node( NODETYPE_CONTROLS_MODE, "Controllers" );
  wii_add_child( controls, child );  

  child = wii_create_tree_node( NODETYPE_WIIMOTE_MODE, "Wiimote mode" );
  wii_add_child( controls, child );  

  child = wii_create_tree_node( NODETYPE_CENTER_ADJUST, "Center adjust" );
  wii_add_child( controls, child );  

  child = wii_create_tree_node( NODETYPE_SENSITIVITY, "Sensitivity adjust" );
  wii_add_child( controls, child );  

  child = wii_create_tree_node( NODETYPE_SMOOTHING, "Smoothing" );
  wii_add_child( controls, child );  

  // Display sub-menu
  TREENODE *display = 
    m_cartSettingsMenuHelper.addDisplaySettingsNode(
      m_cartridgeSettingsMenu );

  // Save/Revert/Delete
  m_cartSettingsMenuHelper.addCartSettingsOpsNodes( m_cartridgeSettingsMenu );
}

extern bool NESIsVSUni;
extern MDFNGI *MDFNGameInfo;

static void getWiimoteModeText( int mode, char * buffer, int size )
{
    switch( mode )
    {
      case NES_WM_MODE_IR:
        snprintf( buffer, size, "IR" );
        break;
      case NES_WM_MODE_ROLL:
        snprintf( buffer, size, "Twist" );
        break;
      case NES_WM_MODE_TILT:
        snprintf( buffer, size, "Tilt" );
        break;
    }
}

void NesMenuManager::getNodeName( 
  TREENODE* node, char *buffer, char* value )
{
  Nes& emu = (Nes&)getEmulator();
  NesDbManager& dbManager = (NesDbManager&)emu.getDbManager();
  NesDbEntry* entry = (NesDbEntry*)dbManager.getEntry();

  // Helpers
  m_emuMenuHelper.getNodeName( node, buffer, value );
  m_cartSettingsMenuHelper.getNodeName( node, buffer, value );

  // Text for the "special" button...
  if( !strcmp( value, "(special)" ) )
  {
    if( NESIsVSUni )
    {
      snprintf( value, WII_MENU_BUFF_SIZE, "Insert coin" );
    }
    else if( MDFNGameInfo->GameType == GMT_DISK )
    {
      snprintf( value, WII_MENU_BUFF_SIZE, "Flip disk" );
    }
#if 0
    else
    {
      snprintf( value, WII_MENU_BUFF_SIZE, 
        NesDbManager::NES_BUTTONS[0].name );
    }
#endif
  }

  switch( node->node_type )
  {        
    case NODETYPE_EMU_WIIMOTE_MODE:
      getWiimoteModeText( emu.getWiimoteMode(), value, WII_MENU_BUFF_SIZE );
      break;
    case NODETYPE_WIIMOTE_MODE:
      switch( entry->wiimoteMode )
      {        
        case NES_WM_MODE_DEFAULT:
          {
            char emuText[WII_MENU_BUFF_SIZE];
            getWiimoteModeText( 
              emu.getWiimoteMode(), emuText, WII_MENU_BUFF_SIZE );
            snprintf( value, WII_MENU_BUFF_SIZE, "(%s, %s)",              
              gettextmsg( emuText ),
              gettextmsg( "emulator" )
            );
          }
          break;
        default:
          getWiimoteModeText( entry->wiimoteMode, value, WII_MENU_BUFF_SIZE );
          break;
      }
      break;
    case NODETYPE_GAME_GENIE:
      snprintf( value, WII_MENU_BUFF_SIZE, 
        emu.isGameGenieEnabled() ? "Enabled" : "Disabled" );
      break;
    case NODETYPE_CONTROLS_MODE:
      switch( dbManager.getControls() )
      {
        case NES_CONTROL_GAMEPADS:
          snprintf( value, WII_MENU_BUFF_SIZE, "Gamepads" );
          break;
        case NES_CONTROL_ZAPPER_P1:
          snprintf( value, WII_MENU_BUFF_SIZE, "Zapper (port 1/Vs.)" );
          break;
        case NES_CONTROL_ZAPPER_P2:
          snprintf( value, WII_MENU_BUFF_SIZE, "Zapper (port 2/NES)" );
          break;
        case NES_CONTROL_ARKANOID_P2:
          snprintf( value, WII_MENU_BUFF_SIZE, "Arkanoid Paddle (port 2)" );
          break;
        case NES_CONTROL_ARKANOID_P5:
          snprintf( value, WII_MENU_BUFF_SIZE, "Arkanoid Paddle (FC Exp.)" );
          break;
        case NES_CONTROL_SHADOW:
          snprintf( value, WII_MENU_BUFF_SIZE, "Space Shadow Gun" );
          break;
        case NES_CONTROL_OEKAKIDS:
          snprintf( value, WII_MENU_BUFF_SIZE, "Oeka Kids Tablet" );
          break;
        case NES_CONTROL_HYPERSHOT:
          snprintf( value, WII_MENU_BUFF_SIZE, "Hypershot" );
          break;
      }
        break;
      case NODETYPE_EMU_CENTER_ADJUST:
        snprintf(  value, WII_MENU_BUFF_SIZE, "%s%d", 
          emu.getCenterAdjust() > 0 ? "+" : "", emu.getCenterAdjust() );
        break;
      case NODETYPE_CENTER_ADJUST:
        {
          if( entry->centerAdjust == NES_CENTER_ADJ_DEFAULT )
          {
            snprintf(  value, WII_MENU_BUFF_SIZE, "(%s%d, %s)", 
              emu.getCenterAdjust() > 0 ? "+" : "", 
              emu.getCenterAdjust(),
              gettextmsg( "emulator" ) );
          }
          else
          {
            snprintf( value, WII_MENU_BUFF_SIZE, "%s%d", 
              entry->centerAdjust > 0 ? "+" : "", entry->centerAdjust );
          }
        }
        break;
      case NODETYPE_EMU_SENSITIVITY:
        snprintf(  value, WII_MENU_BUFF_SIZE, "%s%d", 
          emu.getSensitivity() > 0 ? "+" : "", emu.getSensitivity() );
        break;
      case NODETYPE_SENSITIVITY:
          if( entry->sensitivity == NES_SENSITIVITY_DEFAULT )
          {
            snprintf(  value, WII_MENU_BUFF_SIZE, "(%s%d, %s)", 
              emu.getSensitivity() > 0 ? "+" : "", 
              emu.getSensitivity(),
              gettextmsg( "emulator" ) );
          }
          else
          {
            snprintf( value, WII_MENU_BUFF_SIZE, "%s%d", 
              entry->sensitivity > 0 ? "+" : "", entry->sensitivity );
          }
        break;
      case NODETYPE_EMU_SMOOTHING:
        snprintf(  value, WII_MENU_BUFF_SIZE, "%s", 
          m_cartSettingsMenuHelper.getEnabledText( emu.getSmoothing() ) );
        break;
      case NODETYPE_SMOOTHING:
          if( entry->smoothing == NES_SMOOTHING_DEFAULT )
          {
            snprintf( value, WII_MENU_BUFF_SIZE, "(%s, %s)",                 
              m_cartSettingsMenuHelper.getEnabledText( emu.getSmoothing() ),
              gettextmsg( "emulator" ) );
          }
          else
          {
            snprintf(  value, WII_MENU_BUFF_SIZE, "%s", 
              m_cartSettingsMenuHelper.getEnabledText( entry->smoothing ) );
          }

        break;
  }
}

void NesMenuManager::selectNode( TREENODE *node )
{
  Nes& emu = (Nes&)getEmulator();
  NesDbManager& dbManager = (NesDbManager&)emu.getDbManager();
  NesDbEntry* entry = (NesDbEntry*)dbManager.getEntry();

  // Helpers
  m_emuMenuHelper.selectNode( node );
  m_cartSettingsMenuHelper.selectNode( node );

  LOCK_RENDER_MUTEX();

  switch( node->node_type )
  {
    case NODETYPE_GAME_GENIE:
      emu.setGameGenieEnabled( !emu.isGameGenieEnabled() );
      break;
    case NODETYPE_CONTROLS_MODE:
      {
        int control = dbManager.getControls() + 1;
        if( control >= NES_CONTROL_COUNT  )
        {
          control = 0;
        }
        dbManager.setControls( control );
        // Update the current profile being displayed
        m_cartSettingsMenuHelper.setCurrentProfile( entry->base.profile );

        // Reset the control mappings
        wii_mednafen_set_reset_controls();
      }
      break;
    case NODETYPE_EMU_WIIMOTE_MODE:
      {
        int mode = emu.getWiimoteMode();
        mode++;
        if( mode > NES_WM_MODE_TILT )
        {
          mode = NES_WM_MODE_IR;
        }
        emu.setWiimoteMode( mode );
      }
      break;
    case NODETYPE_WIIMOTE_MODE:
      {
        int mode = entry->wiimoteMode;
        mode++;
        if( mode > NES_WM_MODE_TILT )
        {
          mode = NES_WM_MODE_DEFAULT;
        }
        entry->wiimoteMode = mode;
      }
      break;
    case NODETYPE_EMU_CENTER_ADJUST:
      {
        int center = emu.getCenterAdjust();
        center++;

        if( center > NES_MAX_CENTER_ADJ )
        {
          center = NES_MIN_CENTER_ADJ;
        }
        emu.setCenterAdjust( center );
      }
      break;
    case NODETYPE_CENTER_ADJUST:
      {
        int center = entry->centerAdjust;

        if( center == NES_CENTER_ADJ_DEFAULT )
        {
          center = NES_MIN_CENTER_ADJ;
        }
        else
        {
          center++;
        }

        if( center > NES_MAX_CENTER_ADJ )
        {
          center = NES_CENTER_ADJ_DEFAULT;
        }
        entry->centerAdjust = center;
      }
      break;
    case NODETYPE_EMU_SENSITIVITY:
      {
        int sensitivity = emu.getSensitivity();
        sensitivity++;

        if( sensitivity > NES_MAX_SENSITIVITY )
        {
          sensitivity = NES_MIN_SENSITIVITY;
        }
        emu.setSensitivity( sensitivity );
      }
      break;
    case NODETYPE_SENSITIVITY:
      {
        int sensitivity = entry->sensitivity;
        if( sensitivity == NES_SENSITIVITY_DEFAULT )
        {
          sensitivity = NES_MIN_SENSITIVITY;
        }
        else
        {
          sensitivity++;
        }

        if( sensitivity > NES_MAX_SENSITIVITY )
        {
          sensitivity = NES_SENSITIVITY_DEFAULT;
        }
        entry->sensitivity = sensitivity;
      }
      break;
    case NODETYPE_EMU_SMOOTHING:
      emu.setSmoothing( !emu.getSmoothing() );
      break;
    case NODETYPE_SMOOTHING:
      {
        int smoothing = entry->smoothing;
        smoothing++;
        if( smoothing > 1 )
        {
          smoothing = NES_SMOOTHING_DEFAULT;
        }
        entry->smoothing = smoothing;
      }
      break;
  }

  UNLOCK_RENDER_MUTEX();
}

bool NesMenuManager::isNodeVisible( TREENODE *node )
{
  Nes& emu = (Nes&)getEmulator();
  NesDbManager& dbManager = (NesDbManager&)emu.getDbManager();
  NesDbEntry* entry = (NesDbEntry*)dbManager.getEntry();

  // Helpers
  if( !m_emuMenuHelper.isNodeVisible( node ) ) 
  {
    return false;
  }
  else if( !m_cartSettingsMenuHelper.isNodeVisible( node ) ) 
  {
    return false;
  }

  bool isArkanoid =
    ( dbManager.getControls() == NES_CONTROL_ARKANOID_P2 || 
      dbManager.getControls() == NES_CONTROL_ARKANOID_P5 );

  switch( node->node_type )
  {
    case NODETYPE_EMU_CENTER_ADJUST:
    case NODETYPE_EMU_SENSITIVITY:
    case NODETYPE_EMU_SMOOTHING:
      return emu.getWiimoteMode() != NES_WM_MODE_IR;      
      break;    
    case NODETYPE_WIIMOTE_MODE:
      return isArkanoid;
      break;
    case NODETYPE_CENTER_ADJUST:
    case NODETYPE_SENSITIVITY:
    case NODETYPE_SMOOTHING:
      return isArkanoid && entry->wiimoteMode != NES_WM_MODE_IR;
      break;
  }

  return true;
}