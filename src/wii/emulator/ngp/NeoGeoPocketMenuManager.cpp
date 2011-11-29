#include "main.h"

#include "NeoGeoPocketMenuManager.h"
#include "NeoGeoPocketDbManager.h"
#include "NeoGeoPocket.h"

#include "gettext.h"

#include "wii_app.h"
#include "wii_gx.h"
#include "wii_sdl.h"
#include "wii_mednafen.h"

NeoGeoPocketMenuManager::NeoGeoPocketMenuManager( Emulator &emulator ) :
  MenuManager( emulator ),
  m_emuMenuHelper( emulator ),
  m_cartSettingsMenuHelper( emulator )
{
  //
  // The emulator menu
  // 
  m_emulatorMenu = m_emuMenuHelper.createEmulatorMenu();
  //m_emuMenuHelper.addSpacerNode( m_emulatorMenu );
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

void NeoGeoPocketMenuManager::getNodeName( 
  TREENODE* node, char *buffer, char* value )
{
  NeoGeoPocket& emu = ((NeoGeoPocket&)getEmulator());
  NeoGeoPocketDbManager& dbManager = (NeoGeoPocketDbManager&)emu.getDbManager();
  NeoGeoPocketDbEntry* entry = (NeoGeoPocketDbEntry*)dbManager.getEntry();
  
  // Helpers
  m_emuMenuHelper.getNodeName( node, buffer, value );
  m_cartSettingsMenuHelper.getNodeName( node, buffer, value );

  switch( node->node_type )
  {
    case NODETYPE_GAME_LANGUAGE:
      snprintf( value, WII_MENU_BUFF_SIZE, 
        emu.getGameLanguage() == NGP_LANG_ENGLISH ?
          "English" : "Japanese" );
      break;
  }      
}

void NeoGeoPocketMenuManager::selectNode( TREENODE *node )
{
  NeoGeoPocket& emu = ((NeoGeoPocket&)getEmulator());
  NeoGeoPocketDbManager& dbManager = (NeoGeoPocketDbManager&)emu.getDbManager();
  NeoGeoPocketDbEntry* entry = (NeoGeoPocketDbEntry*)dbManager.getEntry();

  // Helpers
  m_emuMenuHelper.selectNode( node );
  m_cartSettingsMenuHelper.selectNode( node );

  switch( node->node_type )
  {
    case NODETYPE_GAME_LANGUAGE:
      emu.setGameLanguage( !emu.getGameLanguage() );
      break;
  }
}

bool NeoGeoPocketMenuManager::isNodeVisible( TREENODE *node )
{
  NeoGeoPocket& emu = ((NeoGeoPocket&)getEmulator());
  NeoGeoPocketDbManager& dbManager = (NeoGeoPocketDbManager&)emu.getDbManager();
  NeoGeoPocketDbEntry* entry = (NeoGeoPocketDbEntry*)dbManager.getEntry();

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
