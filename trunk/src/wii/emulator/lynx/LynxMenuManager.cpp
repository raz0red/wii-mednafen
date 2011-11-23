#include "main.h"

#include "LynxMenuManager.h"
#include "LynxDbManager.h"
#include "Lynx.h"

#include "gettext.h"

#include "wii_app.h"
#include "wii_gx.h"
#include "wii_sdl.h"
#include "wii_mednafen.h"

LynxMenuManager::LynxMenuManager( Emulator &emulator ) :
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

  m_cartSettingsMenuHelper.addControllerNode( controls );
  m_cartSettingsMenuHelper.addWiimoteSupportedNode( controls );
  m_cartSettingsMenuHelper.addButtonMappingNodes( controls );

  // Display sub-menu
  TREENODE *display = 
    m_cartSettingsMenuHelper.addDisplaySettingsNode(
      m_cartridgeSettingsMenu );

  m_cartSettingsMenuHelper.addSpacerNode( display );
  TREENODE* child = wii_create_tree_node( NODETYPE_ORIENT, "Rotation" );
  wii_add_child( display, child );

  // Save/Revert/Delete
  m_cartSettingsMenuHelper.addCartSettingsOpsNodes( m_cartridgeSettingsMenu );
}

void LynxMenuManager::getNodeName( 
  TREENODE* node, char *buffer, char* value )
{
  Lynx& emu = ((Lynx&)getEmulator());
  LynxDbManager& dbManager = (LynxDbManager&)emu.getDbManager();
  LynxDbEntry* entry = (LynxDbEntry*)dbManager.getEntry();
  
  // Helpers
  m_emuMenuHelper.getNodeName( node, buffer, value );
  m_cartSettingsMenuHelper.getNodeName( node, buffer, value );

  switch( node->node_type )
  {
    case NODETYPE_ORIENT:
      snprintf( value, WII_MENU_BUFF_SIZE, "%s",
        ( entry->orient == MDFN_ROTATE270 ?  "270 degrees" : 
          ( entry->orient == MDFN_ROTATE90 ? "90 degrees" : "None" ) ) );
      break;
  }
}

void LynxMenuManager::selectNode( TREENODE *node )
{
  Lynx& emu = ((Lynx&)getEmulator());
  LynxDbManager& dbManager = (LynxDbManager&)emu.getDbManager();
  LynxDbEntry* entry = (LynxDbEntry*)dbManager.getEntry();

  // Helpers
  m_emuMenuHelper.selectNode( node );
  m_cartSettingsMenuHelper.selectNode( node );

  LOCK_RENDER_MUTEX();

  switch( node->node_type )
  {
    case NODETYPE_ORIENT:
      switch( entry->orient )
      {
        case MDFN_ROTATE0:
          entry->orient = MDFN_ROTATE90;
          break;
        case MDFN_ROTATE90:
          entry->orient = MDFN_ROTATE270;
          break;
        default:
          entry->orient = MDFN_ROTATE0;
          break;
      }
      break;
  }

  UNLOCK_RENDER_MUTEX();
}

bool LynxMenuManager::isNodeVisible( TREENODE *node )
{
  Lynx& emu = ((Lynx&)getEmulator());
  LynxDbManager& dbManager = (LynxDbManager&)emu.getDbManager();
  LynxDbEntry* entry = (LynxDbEntry*)dbManager.getEntry();

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
