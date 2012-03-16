#include "CartridgeSettingsMenuHelper.h"
#include "DatabaseManager.h"

#include "gettext.h"

#include "wii_gx.h"
#include "wii_sdl.h"
#include "wii_app.h"
#include "wii_mednafen.h"

CartridgeSettingsMenuHelper::CartridgeSettingsMenuHelper( Emulator& emulator ) :
  BaseMenuHelper( emulator )
{
}

TREENODE* CartridgeSettingsMenuHelper::createCartridgeSettingsMenu()
{
  return wii_create_tree_node( NODETYPE_CARTRIDGE_SETTINGS_CURRENT, "" );
}

TREENODE* CartridgeSettingsMenuHelper::addControlsSettingsNode( 
  TREENODE* parent )
{
  TREENODE *controls = wii_create_tree_node( 
    NODETYPE_CARTRIDGE_SETTINGS_CONTROLS, "Control settings" );                                                        
  wii_add_child( parent, controls );
  return controls;
}

TREENODE* CartridgeSettingsMenuHelper::addControlsMappingsNode( 
  TREENODE* parent )
{
  TREENODE *mappings = wii_create_tree_node( 
    NODETYPE_CARTRIDGE_SETTINGS_MAPPINGS, "Button mappings" );
  wii_add_child( parent, mappings );

  return mappings;
}


TREENODE* CartridgeSettingsMenuHelper::addDisplaySettingsNode( 
  TREENODE* parent )
{
  TREENODE *cartDisplay = wii_create_tree_node( 
    NODETYPE_CARTRIDGE_SETTINGS_DISPLAY, "Display settings" );                                                        
  wii_add_child( parent, cartDisplay );

  wii_add_child( cartDisplay, 
    wii_create_tree_node( NODETYPE_CART_FRAME_SKIP, "Frame skip" ) );

  return cartDisplay;
}

TREENODE* CartridgeSettingsMenuHelper::addAdvancedSettingsNode( 
  TREENODE* parent )
{
  TREENODE *cartadvanced = wii_create_tree_node( 
    NODETYPE_CARTRIDGE_SETTINGS_ADVANCED, "Advanced" );
  wii_add_child( parent, cartadvanced );

  return cartadvanced;
}

void CartridgeSettingsMenuHelper::addCartSettingsOpsNodes( 
  TREENODE* parent )
{
  addSpacerNode( parent );

  TREENODE* child = wii_create_tree_node( 
    NODETYPE_SAVE_CARTRIDGE_SETTINGS, "Save settings" );
  wii_add_child( parent, child );  

  child = wii_create_tree_node( 
    NODETYPE_REVERT_CARTRIDGE_SETTINGS, "Revert to saved settings" );
  wii_add_child( parent, child );  

  child = wii_create_tree_node( 
    NODETYPE_DELETE_CARTRIDGE_SETTINGS, "Delete settings" );
  wii_add_child( parent, child );  
}

void CartridgeSettingsMenuHelper::getNodeName( 
  TREENODE* node, char *buffer, char* value )
{
  Emulator& emu = getEmulator();
  DatabaseManager& dbManager = emu.getDbManager();

  switch( node->node_type )
  {
    case NODETYPE_CART_FRAME_SKIP:
    { 
      dbEntry* entry = dbManager.getEntry();
      switch( entry->frameSkip )
      {
        case FRAME_SKIP_DEFAULT:
          snprintf( value, WII_MENU_BUFF_SIZE, "(%s, %s)",              
            gettextmsg( getEnabledText( emu.getFrameSkip() ) ),
            gettextmsg( "emulator" ) );
          break;
        default:
           snprintf( value, WII_MENU_BUFF_SIZE, "%s", 
             getEnabledText( entry->frameSkip == FRAME_SKIP_ENABLED ) );
          break;
      }     
    }
    break;
  }
}

void CartridgeSettingsMenuHelper::selectNode( TREENODE* node )
{
  Emulator& emu = getEmulator();
  DatabaseManager& dbManager = emu.getDbManager();

  LOCK_RENDER_MUTEX();

  switch( node->node_type )
  {
    case NODETYPE_CARTRIDGE_SETTINGS_DISPLAY:
    case NODETYPE_CARTRIDGE_SETTINGS_CONTROLS:
    case NODETYPE_CARTRIDGE_SETTINGS_ADVANCED:
    case NODETYPE_CARTRIDGE_SETTINGS_MAPPINGS:
      wii_menu_push( node );
      break;
    case NODETYPE_CART_FRAME_SKIP:
      {
        dbEntry* entry = dbManager.getEntry();
        entry->frameSkip++;
        if( entry->frameSkip > FRAME_SKIP_DISABLED )
        {
          entry->frameSkip = FRAME_SKIP_DEFAULT;
        }
      }
      break;
    case NODETYPE_SAVE_CARTRIDGE_SETTINGS:
      {
        if( dbManager.getEntry()->name[0] == '\0' )
        {
          char cartname[WII_MAX_PATH];        
          Util_splitpath( wii_last_rom, NULL, cartname );
          char *ptr = strrchr( cartname, '.' );
          if( ptr ) *ptr = '\0';
          Util_strlcpy( 
            dbManager.getEntry()->name, cartname, 
            sizeof( dbManager.getEntry()->name ) );
        }
        if( dbManager.writeEntry( wii_cartridge_hash ) )
        {
          dbManager.getEntry()->loaded = 1;
          wii_set_status_message( "Successfully saved cartridge settings." );
        }
        else
        {
          wii_set_status_message( 
            "An error occurred saving cartridge settings." );
        }
      }
      break;
    case NODETYPE_DELETE_CARTRIDGE_SETTINGS:
      {
        if( dbManager.deleteEntry( wii_cartridge_hash ) )
        {
          wii_menu_reset_indexes();
          wii_menu_move( wii_menu_stack[wii_menu_stack_head], 1 );
          wii_set_status_message( "Successfully deleted cartridge settings." );
        }
        else
        {
          wii_set_status_message( 
            "An error occurred deleting cartridge settings." );
        }

        // Load the values for the entry 
        dbManager.loadEntry( wii_cartridge_hash );
      }
      break;
    case NODETYPE_REVERT_CARTRIDGE_SETTINGS:
      {
        dbManager.loadEntry( wii_cartridge_hash );        
        wii_set_status_message( "Successfully reverted to saved settings." );
      }
      break;
  }

  UNLOCK_RENDER_MUTEX();
}

bool CartridgeSettingsMenuHelper::isNodeVisible( TREENODE* node )
{
  Emulator& emu = getEmulator();
  DatabaseManager& dbManager = emu.getDbManager();

  switch( node->node_type )
  {
    case NODETYPE_DELETE_CARTRIDGE_SETTINGS:
    case NODETYPE_REVERT_CARTRIDGE_SETTINGS:
      return wii_last_rom != NULL && dbManager.getEntry()->loaded;
  }

  return true;
}