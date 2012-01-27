#include "main.h"

#include "GameBoyAdvanceMenuManager.h"
#include "GameBoyAdvanceDbManager.h"
#include "GameBoyAdvance.h"

#include "gettext.h"

#include "wii_app.h"
#include "wii_gx.h"
#include "wii_sdl.h"
#include "wii_mednafen.h"

GameBoyAdvanceMenuManager::GameBoyAdvanceMenuManager( Emulator &emulator ) :
  MenuManager( emulator ),
  m_emuMenuHelper( emulator ),
  m_cartSettingsMenuHelper( emulator )
{
  //
  // The emulator menu
  // 
  m_emulatorMenu = m_emuMenuHelper.createEmulatorMenu();

  //m_emuMenuHelper.addSpacerNode( m_emulatorMenu );

  TREENODE* child = wii_create_tree_node( NODETYPE_GBA_BIOS, "External BIOS" );
  wii_add_child( m_emulatorMenu, child );

  //
  // The cartridge settings (current) menu
  //
  m_cartridgeSettingsMenu = 
    m_cartSettingsMenuHelper.createCartridgeSettingsMenu();   

  // Controls sub-menu
  TREENODE *controls = 
    m_cartSettingsMenuHelper.addControlsSettingsNode( 
      m_cartridgeSettingsMenu );

  m_cartSettingsMenuHelper.addControllerNode( controls );
  m_cartSettingsMenuHelper.addWiimoteSupportedNode( controls );
  m_cartSettingsMenuHelper.addButtonMappingNodes( controls );

  // Display sub-menu
  TREENODE *display = 
    m_cartSettingsMenuHelper.addDisplaySettingsNode(
      m_cartridgeSettingsMenu );

  // Save/Revert/Delete
  m_cartSettingsMenuHelper.addCartSettingsOpsNodes( m_cartridgeSettingsMenu );
}

void GameBoyAdvanceMenuManager::getNodeName( 
  TREENODE* node, char *buffer, char* value )
{
  GameBoyAdvance& emu = (GameBoyAdvance&)getEmulator();
  GameBoyAdvanceDbManager& dbManager = 
    (GameBoyAdvanceDbManager&)emu.getDbManager();
  GameBoyAdvanceDbEntry* entry = 
    (GameBoyAdvanceDbEntry*)dbManager.getEntry();

  // Helpers
  m_emuMenuHelper.getNodeName( node, buffer, value );
  m_cartSettingsMenuHelper.getNodeName( node, buffer, value );

  switch( node->node_type )
  {
    case NODETYPE_GBA_BIOS:
      {
        BOOL enabled = FALSE;
        switch( node->node_type )
        {
          case NODETYPE_GBA_BIOS:
            enabled = emu.isGbaBiosEnabled(); 
            break;
          default:
            /* do nothing */
            break;
        }
        snprintf( value, WII_MENU_BUFF_SIZE, "%s", 
          enabled ? "Enabled" : "Disabled" );
        break;
      }
  }
}

void GameBoyAdvanceMenuManager::selectNode( TREENODE *node )
{
  GameBoyAdvance& emu = (GameBoyAdvance&)getEmulator();
  GameBoyAdvanceDbManager& dbManager = 
    (GameBoyAdvanceDbManager&)emu.getDbManager();
  GameBoyAdvanceDbEntry* entry = 
    (GameBoyAdvanceDbEntry*)dbManager.getEntry();

  // Helpers
  m_emuMenuHelper.selectNode( node );
  m_cartSettingsMenuHelper.selectNode( node );

  LOCK_RENDER_MUTEX();

  switch( node->node_type )
  {
    case NODETYPE_GBA_BIOS:
      emu.setGbaBiosEnabled( !emu.isGbaBiosEnabled() );
      break;
  }

  UNLOCK_RENDER_MUTEX();
}

bool GameBoyAdvanceMenuManager::isNodeVisible( TREENODE *node )
{
  GameBoyAdvance& emu = (GameBoyAdvance&)getEmulator();
  GameBoyAdvanceDbManager& dbManager = 
    (GameBoyAdvanceDbManager&)emu.getDbManager();
  GameBoyAdvanceDbEntry* entry = 
    (GameBoyAdvanceDbEntry*)dbManager.getEntry();

  // Helpers
  if( !m_emuMenuHelper.isNodeVisible( node ) ) 
  {
    return false;
  }
  else if( !m_cartSettingsMenuHelper.isNodeVisible( node ) ) 
  {
    return false;
  }

  return true;
}
