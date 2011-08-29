#include "main.h"

#include "NesMenuManager.h"
#include "NesDbManager.h"
#include "Nes.h"

#include "gettext.h"

#include "wii_app.h"
#include "wii_gx.h"
#include "wii_sdl.h"
#include "wii_resize_screen.h"
#include "wii_mednafen.h"

NesMenuManager::NesMenuManager( Emulator &emulator ) :
  MenuManager( emulator ),
  m_emuMenuHelper( emulator ),
  m_cartSettingsMenuHelper( emulator )
{
  //
  // The emulator menu
  // 
  m_emulatorMenu = m_emuMenuHelper.createEmulatorMenu();

  m_emuMenuHelper.addSpacerNode( m_emulatorMenu );

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

extern bool NESIsVSUni;
extern MDFNGI *MDFNGameInfo;

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
    case NODETYPE_GAME_GENIE:
      snprintf( value, WII_MENU_BUFF_SIZE, 
        emu.isGameGenieEnabled() ? "Enabled" : "Disabled" );
      break;
  }
}

void NesMenuManager::selectNode( TREENODE *node )
{
  char buff[WII_MAX_PATH];

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

  return true;
}
