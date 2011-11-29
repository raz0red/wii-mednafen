#include "main.h"

#include "MegaDriveMenuManager.h"
#include "MegaDriveDbManager.h"
#include "MegaDrive.h"

#include "gettext.h"

#include "wii_app.h"
#include "wii_gx.h"
#include "wii_sdl.h"
#include "wii_mednafen.h"

MegaDriveMenuManager::MegaDriveMenuManager( Emulator &emulator ) :
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

  // Control mappings
  TREENODE *mappings = 
    m_cartSettingsMenuHelper.addControlsMappingsNode( 
      controls );

  m_cartSettingsMenuHelper.addControllerNode( mappings );
  m_cartSettingsMenuHelper.addWiimoteSupportedNode( mappings );
  m_cartSettingsMenuHelper.addButtonMappingNodes( mappings );

  m_cartSettingsMenuHelper.addSpacerNode( controls );

  for( int i = NODETYPE_CONTROL_TYPE1; i <= NODETYPE_CONTROL_TYPE2; i++ )
  {
    char buff[512];
    snprintf( buff, sizeof(buff), "Controller %d", 
      ( i - NODETYPE_CONTROL_TYPE1 ) + 1 );
    wii_add_child( controls, 
      wii_create_tree_node( (NODETYPE)i, buff ) );
  }

  // Display sub-menu
  TREENODE *display = 
    m_cartSettingsMenuHelper.addDisplaySettingsNode(
      m_cartridgeSettingsMenu );

  // Save/Revert/Delete
  m_cartSettingsMenuHelper.addCartSettingsOpsNodes( m_cartridgeSettingsMenu );
}

void MegaDriveMenuManager::getNodeName( 
  TREENODE* node, char *buffer, char* value )
{
  MegaDrive& emu = (MegaDrive&)getEmulator();
  MegaDriveDbManager& dbManager = (MegaDriveDbManager&)emu.getDbManager();
  MegaDriveDbEntry* entry = (MegaDriveDbEntry*)dbManager.getEntry();

  // Helpers
  m_emuMenuHelper.getNodeName( node, buffer, value );
  m_cartSettingsMenuHelper.getNodeName( node, buffer, value );

  switch( node->node_type )
  {
    case NODETYPE_CONTROL_TYPE1:
    case NODETYPE_CONTROL_TYPE2:
      {
        int index = node->node_type - NODETYPE_CONTROL_TYPE1;
        snprintf( value, WII_MENU_BUFF_SIZE, "%d-%s",          
          ( entry->controlType[index] == MD_CONTROL_3BUTTON ? 3 : 6 ),
          gettextmsg( "button pad" ) );
      }
      break;
    case NODETYPE_MD_REGION:
      snprintf( value, WII_MENU_BUFF_SIZE, "%s", emu.getConsoleRegionName() );
      break;
  }
}

void MegaDriveMenuManager::selectNode( TREENODE *node )
{
  MegaDrive& emu = (MegaDrive&)getEmulator();
  MegaDriveDbManager& dbManager = (MegaDriveDbManager&)emu.getDbManager();
  MegaDriveDbEntry* entry = (MegaDriveDbEntry*)dbManager.getEntry();

  // Helpers
  m_emuMenuHelper.selectNode( node );
  m_cartSettingsMenuHelper.selectNode( node );

  LOCK_RENDER_MUTEX();

  switch( node->node_type )
  {
    case NODETYPE_CONTROL_TYPE1:
    case NODETYPE_CONTROL_TYPE2:
      {
        int index = node->node_type - NODETYPE_CONTROL_TYPE1;
        entry->controlType[index] = !entry->controlType[index];
      }
      break;
    case NODETYPE_MD_REGION:
      {
        int region = emu.getConsoleRegion() + 1;
        if( region >= MegaDrive::regionCount )
        {
          region = 0;
        }
        emu.setConsoleRegion( region );
      }
      break;
  }

  UNLOCK_RENDER_MUTEX();
}

bool MegaDriveMenuManager::isNodeVisible( TREENODE *node )
{
  MegaDrive& emu = (MegaDrive&)getEmulator();
  MegaDriveDbManager& dbManager = (MegaDriveDbManager&)emu.getDbManager();
  MegaDriveDbEntry* entry = (MegaDriveDbEntry*)dbManager.getEntry();

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
