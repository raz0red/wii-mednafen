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

void NesMenuManager::getNodeName( 
  TREENODE* node, char *buffer, char* value )
{
  Nes& emu = (Nes&)getEmulator();
  NesDbManager& dbManager = (NesDbManager&)emu.getDbManager();
  NesDbEntry* entry = (NesDbEntry*)dbManager.getEntry();

  // Helpers
  m_emuMenuHelper.getNodeName( node, buffer, value );
  m_cartSettingsMenuHelper.getNodeName( node, buffer, value );
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
