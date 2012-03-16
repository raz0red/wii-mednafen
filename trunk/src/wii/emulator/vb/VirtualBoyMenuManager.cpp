#include "VirtualBoyMenuManager.h"
#include "VirtualBoyDbManager.h"
#include "VirtualBoy.h"

#include "gettext.h"

#include "wii_app.h"
#include "wii_gx.h"
#include "wii_sdl.h"
#include "wii_mednafen.h"

VirtualBoyMenuManager::VirtualBoyMenuManager( Emulator &emulator ) :
  MenuManager( emulator ),
  m_emuMenuHelper( emulator ),
  m_cartSettingsMenuHelper( emulator )
{
  //
  // The emulator menu
  // 
  m_emulatorMenu = m_emuMenuHelper.createEmulatorMenu();

  //m_emuMenuHelper.addSpacerNode( m_emulatorMenu );

  TREENODE* child = wii_create_tree_node( NODETYPE_VB_MODE, "Display mode" );
  wii_add_child( m_emulatorMenu, child );

  child = wii_create_tree_node( NODETYPE_ROM_PATCH, "ROM patching" );
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

  // Advanced sub-menu
  TREENODE *cartadvanced = 
    m_cartSettingsMenuHelper.addAdvancedSettingsNode(
      m_cartridgeSettingsMenu );

  child = wii_create_tree_node( NODETYPE_ROM_PATCH_CART, "ROM patching" );
  wii_add_child( cartadvanced, child );  

  // Save/Revert/Delete
  m_cartSettingsMenuHelper.addCartSettingsOpsNodes( m_cartridgeSettingsMenu );
}

void VirtualBoyMenuManager::getNodeName( 
  TREENODE* node, char *buffer, char* value )
{
  VirtualBoy& emu = ((VirtualBoy&)getEmulator());
  VirtualBoyDbManager& dbManager = (VirtualBoyDbManager&)emu.getDbManager();
  VbDbEntry* entry = (VbDbEntry*)dbManager.getEntry();
  
  // Helpers
  m_emuMenuHelper.getNodeName( node, buffer, value );
  m_cartSettingsMenuHelper.getNodeName( node, buffer, value );

  switch( node->node_type )
  {
    case NODETYPE_ROM_PATCH:
      {
        BOOL enabled = FALSE;
        switch( node->node_type )
        {
          case NODETYPE_ROM_PATCH:
            enabled = emu.getPatchRom(); 
            break;
          default:
            /* do nothing */
            break;
        }
        snprintf( value, WII_MENU_BUFF_SIZE, "%s", 
          enabled ? "Enabled" : "Disabled" );
        break;
      }
    case NODETYPE_VB_MODE:
      {
        snprintf( value, WII_MENU_BUFF_SIZE, "%s", emu.getMode().name );
      }
      break;
    case NODETYPE_ROM_PATCH_CART:
      {
        switch( entry->romPatch )
        {
          case ROM_PATCH_ENABLED:
          case ROM_PATCH_DISABLED:
            snprintf( value, WII_MENU_BUFF_SIZE, "%s", 
                m_emuMenuHelper.getEnabledText( 
                  entry->romPatch == ROM_PATCH_ENABLED ) );
            break;
          default:
            snprintf( value, WII_MENU_BUFF_SIZE, "(%s, %s)", 
              gettextmsg( 
                m_emuMenuHelper.getEnabledText( emu.getPatchRom() ) ),
              gettextmsg( "emulator" ) );
            break;
        }
      }
      break;
  }
}

void VirtualBoyMenuManager::selectNode( TREENODE *node )
{
  VirtualBoy& emu = ((VirtualBoy&)getEmulator());
  VirtualBoyDbManager& dbManager = (VirtualBoyDbManager&)emu.getDbManager();
  VbDbEntry* entry = (VbDbEntry*)dbManager.getEntry();

  // Helpers
  m_emuMenuHelper.selectNode( node );
  m_cartSettingsMenuHelper.selectNode( node );

  LOCK_RENDER_MUTEX();

  switch( node->node_type )
  {
    case NODETYPE_VB_MODE:
      {
        const Vb3dMode* mode = NULL;
        int index = emu.getModeIndex();
        while( 1 )
        {
          index++;
          if( index >= VirtualBoy::VB_MODE_COUNT )
          {
            index = 0;
          }
          
          mode = &VirtualBoy::VB_MODES[index];
          if( !emu.isCustomMode( mode ) || emu.hasCustomColors() )
          { 
            break;
          }                   
        }

        emu.setMode( mode->key );
      }
      break;
    case NODETYPE_ROM_PATCH_CART:
      entry->romPatch++;
      if( entry->romPatch > ROM_PATCH_DISABLED )
      {
        entry->romPatch = 0;
      }
      break;
    case NODETYPE_ROM_PATCH:
      emu.setPatchRom( !emu.getPatchRom() );
      break;
  }

  UNLOCK_RENDER_MUTEX();
}

bool VirtualBoyMenuManager::isNodeVisible( TREENODE *node )
{
  VirtualBoy& emu = ((VirtualBoy&)getEmulator());
  VirtualBoyDbManager& dbManager = (VirtualBoyDbManager&)emu.getDbManager();
  VbDbEntry* entry = (VbDbEntry*)dbManager.getEntry();

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
