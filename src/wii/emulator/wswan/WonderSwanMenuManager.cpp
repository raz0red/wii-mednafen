#include "WonderSwanMenuManager.h"
#include "WonderSwanDbManager.h"
#include "WonderSwan.h"

#include "gettext.h"

#include "wii_app.h"
#include "wii_gx.h"
#include "wii_sdl.h"
#include "wii_mednafen.h"

WonderSwanMenuManager::WonderSwanMenuManager( Emulator &emulator ) :
  MenuManager( emulator ),
  m_emuMenuHelper( emulator ),
  m_cartSettingsMenuHelper( emulator )
{
  //
  // The emulator menu
  // 
  m_emulatorMenu = m_emuMenuHelper.createEmulatorMenu();
  TREENODE* child = wii_create_tree_node( NODETYPE_GAME_LANGUAGE, "Language (game)" );
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

  m_cartSettingsMenuHelper.addProfileNode( controls );
  m_cartSettingsMenuHelper.addControllerNode( controls );
  m_cartSettingsMenuHelper.addWiimoteSupportedNode( controls );
  m_cartSettingsMenuHelper.addButtonMappingNodes( controls );

  // Display sub-menu
  TREENODE *display = 
    m_cartSettingsMenuHelper.addDisplaySettingsNode(
      m_cartridgeSettingsMenu );

  m_cartSettingsMenuHelper.addSpacerNode( display );
  child = wii_create_tree_node( NODETYPE_ORIENT, "Rotation" );
  wii_add_child( display, child );

  // Save/Revert/Delete
  m_cartSettingsMenuHelper.addCartSettingsOpsNodes( m_cartridgeSettingsMenu );
}

void WonderSwanMenuManager::getNodeName( 
  TREENODE* node, char *buffer, char* value )
{
  WonderSwan& emu = ((WonderSwan&)getEmulator());
  WonderSwanDbManager& dbManager = (WonderSwanDbManager&)emu.getDbManager();
  StandardDbEntry* entry = (StandardDbEntry*)dbManager.getEntry();
  
  // Helpers
  m_emuMenuHelper.getNodeName( node, buffer, value );
  m_cartSettingsMenuHelper.getNodeName( node, buffer, value );

  switch( node->node_type )
  {
    case NODETYPE_ORIENT:
      snprintf( value, WII_MENU_BUFF_SIZE, "%s",
        ( entry->profile ?  "90 degrees" : "None" ) );
      break;
    case NODETYPE_GAME_LANGUAGE:
      snprintf( value, WII_MENU_BUFF_SIZE, 
        emu.getGameLanguage() == WS_LANG_ENGLISH ?
          "English" : "Japanese" );
      break;
  }
}

void WonderSwanMenuManager::selectNode( TREENODE *node )
{
  WonderSwan& emu = ((WonderSwan&)getEmulator());
  WonderSwanDbManager& dbManager = (WonderSwanDbManager&)emu.getDbManager();
  StandardDbEntry* entry = (StandardDbEntry*)dbManager.getEntry();

  // Helpers
  m_emuMenuHelper.selectNode( node );
  m_cartSettingsMenuHelper.selectNode( node );

  LOCK_RENDER_MUTEX();

  switch( node->node_type )
  {
    case NODETYPE_ORIENT:
      entry->profile ^= 1;
      break;
    case NODETYPE_GAME_LANGUAGE:
      emu.setGameLanguage( !emu.getGameLanguage() );
      break;
  }

  UNLOCK_RENDER_MUTEX();
}

bool WonderSwanMenuManager::isNodeVisible( TREENODE *node )
{
  WonderSwan& emu = ((WonderSwan&)getEmulator());
  WonderSwanDbManager& dbManager = (WonderSwanDbManager&)emu.getDbManager();
  WswanDbEntry* entry = (WswanDbEntry*)dbManager.getEntry();

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
