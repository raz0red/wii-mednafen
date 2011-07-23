#include "main.h"

#include "GameBoyAdvanceMenuManager.h"
#include "GameBoyAdvanceDbManager.h"
#include "GameBoyAdvance.h"

#include "gettext.h"

#include "wii_app.h"
#include "wii_gx.h"
#include "wii_sdl.h"
#include "wii_resize_screen.h"
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
}

void GameBoyAdvanceMenuManager::selectNode( TREENODE *node )
{
  char buff[WII_MAX_PATH];

  GameBoyAdvance& emu = (GameBoyAdvance&)getEmulator();
  GameBoyAdvanceDbManager& dbManager = 
    (GameBoyAdvanceDbManager&)emu.getDbManager();
  GameBoyAdvanceDbEntry* entry = 
    (GameBoyAdvanceDbEntry*)dbManager.getEntry();

  // Helpers
  m_emuMenuHelper.selectNode( node );
  m_cartSettingsMenuHelper.selectNode( node );
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
