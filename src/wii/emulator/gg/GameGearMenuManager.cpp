#include "main.h"

#include "GameGearMenuManager.h"
#include "GameGearDbManager.h"
#include "GameGear.h"

#include "gettext.h"

#include "wii_app.h"
#include "wii_gx.h"
#include "wii_sdl.h"
#include "wii_mednafen.h"

GameGearMenuManager::GameGearMenuManager( Emulator &emulator ) :
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

  // Save/Revert/Delete
  m_cartSettingsMenuHelper.addCartSettingsOpsNodes( m_cartridgeSettingsMenu );
}

void GameGearMenuManager::getNodeName( 
  TREENODE* node, char *buffer, char* value )
{
  GameGear& emu = ((GameGear&)getEmulator());
  GameGearDbManager& dbManager = (GameGearDbManager&)emu.getDbManager();
  GameGearDbEntry* entry = (GameGearDbEntry*)dbManager.getEntry();
  
  // Helpers
  m_emuMenuHelper.getNodeName( node, buffer, value );
  m_cartSettingsMenuHelper.getNodeName( node, buffer, value );
}

void GameGearMenuManager::selectNode( TREENODE *node )
{
  GameGear& emu = ((GameGear&)getEmulator());
  GameGearDbManager& dbManager = (GameGearDbManager&)emu.getDbManager();
  GameGearDbEntry* entry = (GameGearDbEntry*)dbManager.getEntry();

  // Helpers
  m_emuMenuHelper.selectNode( node );
  m_cartSettingsMenuHelper.selectNode( node );
}

bool GameGearMenuManager::isNodeVisible( TREENODE *node )
{
  GameGear& emu = ((GameGear&)getEmulator());
  GameGearDbManager& dbManager = (GameGearDbManager&)emu.getDbManager();
  GameGearDbEntry* entry = (GameGearDbEntry*)dbManager.getEntry();

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
