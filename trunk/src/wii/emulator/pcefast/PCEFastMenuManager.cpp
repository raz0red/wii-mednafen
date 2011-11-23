#include "main.h"

#include "PCEFastMenuManager.h"
#include "PCEFastDbManager.h"
#include "PCEFast.h"

#include "gettext.h"

#include "wii_app.h"
#include "wii_gx.h"
#include "wii_sdl.h"
#include "wii_mednafen.h"

PCEFastMenuManager::PCEFastMenuManager( Emulator &emulator ) :
  MenuManager( emulator ),
  m_emuMenuHelper( emulator ),
  m_cartSettingsMenuHelper( emulator )
{
  //
  // The emulator menu
  // 
  m_emulatorMenu = m_emuMenuHelper.createEmulatorMenu();

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

  for( int i = NODETYPE_CONTROL_TYPE1; i <= NODETYPE_CONTROL_TYPE4; i++ )
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

void PCEFastMenuManager::getNodeName( 
  TREENODE* node, char *buffer, char* value )
{
  PCEFast& emu = (PCEFast&)getEmulator();
  PCEFastDbManager& dbManager = (PCEFastDbManager&)emu.getDbManager();
  PCEFastDbEntry* entry = (PCEFastDbEntry*)dbManager.getEntry();

  // Helpers
  m_emuMenuHelper.getNodeName( node, buffer, value );
  m_cartSettingsMenuHelper.getNodeName( node, buffer, value );

  switch( node->node_type )
  {
    case NODETYPE_CONTROL_TYPE1:
    case NODETYPE_CONTROL_TYPE2:
    case NODETYPE_CONTROL_TYPE3:
    case NODETYPE_CONTROL_TYPE4:
      {
        int index = node->node_type - NODETYPE_CONTROL_TYPE1;
        snprintf( value, WII_MENU_BUFF_SIZE, "%d-%s",
          ( entry->controlType[index] == PCE_CONTROL_2BUTTON ? 2 : 6 ),
          gettextmsg( "button pad" ) );
      }
    break;
  }
}

void PCEFastMenuManager::selectNode( TREENODE *node )
{
  PCEFast& emu = (PCEFast&)getEmulator();
  PCEFastDbManager& dbManager = (PCEFastDbManager&)emu.getDbManager();
  PCEFastDbEntry* entry = (PCEFastDbEntry*)dbManager.getEntry();

  // Helpers
  m_emuMenuHelper.selectNode( node );
  m_cartSettingsMenuHelper.selectNode( node );

  LOCK_RENDER_MUTEX();

  switch( node->node_type )
  {
    case NODETYPE_CONTROL_TYPE1:
    case NODETYPE_CONTROL_TYPE2:
    case NODETYPE_CONTROL_TYPE3:
    case NODETYPE_CONTROL_TYPE4:
      {
        int index = node->node_type - NODETYPE_CONTROL_TYPE1;
        entry->controlType[index] = !(entry->controlType[index]);
      }
    break;
  }

  UNLOCK_RENDER_MUTEX();
}

bool PCEFastMenuManager::isNodeVisible( TREENODE *node )
{
  PCEFast& emu = (PCEFast&)getEmulator();
  PCEFastDbManager& dbManager = (PCEFastDbManager&)emu.getDbManager();
  PCEFastDbEntry* entry = (PCEFastDbEntry*)dbManager.getEntry();

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
