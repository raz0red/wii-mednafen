#include "main.h"

#include "MasterSystemMenuManager.h"
#include "MasterSystemDbManager.h"
#include "MasterSystem.h"

#include "gettext.h"

#include "wii_app.h"
#include "wii_gx.h"
#include "wii_sdl.h"
#include "wii_mednafen.h"

MasterSystemMenuManager::MasterSystemMenuManager( Emulator &emulator ) :
  MenuManager( emulator ),
  m_emuMenuHelper( emulator ),
  m_cartSettingsMenuHelper( emulator )
{
  //
  // The emulator menu
  // 
  m_emulatorMenu = m_emuMenuHelper.createEmulatorMenu();

  //m_emuMenuHelper.addSpacerNode( m_emulatorMenu );

  TREENODE* child = wii_create_tree_node( NODETYPE_MD_REGION, "Console region" );
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

void MasterSystemMenuManager::getNodeName( 
  TREENODE* node, char *buffer, char* value )
{
  MasterSystem& emu = ((MasterSystem&)getEmulator());
  MasterSystemDbManager& dbManager = (MasterSystemDbManager&)emu.getDbManager();
  MasterSystemDbEntry* entry = (MasterSystemDbEntry*)dbManager.getEntry();
  
  // Helpers
  m_emuMenuHelper.getNodeName( node, buffer, value );
  m_cartSettingsMenuHelper.getNodeName( node, buffer, value );

  switch( node->node_type )
  {
    case NODETYPE_MD_REGION:
      snprintf( value, WII_MENU_BUFF_SIZE, "%s", emu.getConsoleRegionName() );
      break;
  }
}

void MasterSystemMenuManager::selectNode( TREENODE *node )
{
  MasterSystem& emu = ((MasterSystem&)getEmulator());
  MasterSystemDbManager& dbManager = (MasterSystemDbManager&)emu.getDbManager();
  MasterSystemDbEntry* entry = (MasterSystemDbEntry*)dbManager.getEntry();

  // Helpers
  m_emuMenuHelper.selectNode( node );
  m_cartSettingsMenuHelper.selectNode( node );

  LOCK_RENDER_MUTEX();

  switch( node->node_type )
  {
    case NODETYPE_MD_REGION:
      {
        int region = emu.getConsoleRegion() + 1;
        if( region >= MasterSystem::regionCount )
        {
          region = 0;
        }
        emu.setConsoleRegion( region );
      }
      break;
  }

  UNLOCK_RENDER_MUTEX();
}

bool MasterSystemMenuManager::isNodeVisible( TREENODE *node )
{
  MasterSystem& emu = ((MasterSystem&)getEmulator());
  MasterSystemDbManager& dbManager = (MasterSystemDbManager&)emu.getDbManager();
  MasterSystemDbEntry* entry = (MasterSystemDbEntry*)dbManager.getEntry();

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
