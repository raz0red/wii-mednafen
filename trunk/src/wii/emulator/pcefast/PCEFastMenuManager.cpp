#include "main.h"

#include "PCEFastMenuManager.h"
#include "PCEFastDbManager.h"
#include "PCEFast.h"

#include "gettext.h"

#include "wii_app.h"
#include "wii_gx.h"
#include "wii_sdl.h"
#include "wii_resize_screen.h"
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

  m_cartSettingsMenuHelper.addControllerNode( controls );
  m_cartSettingsMenuHelper.addWiimoteSupportedNode( controls );
  m_cartSettingsMenuHelper.addButtonMappingNodes( controls );

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
}

void PCEFastMenuManager::selectNode( TREENODE *node )
{
  char buff[WII_MAX_PATH];

  PCEFast& emu = (PCEFast&)getEmulator();
  PCEFastDbManager& dbManager = (PCEFastDbManager&)emu.getDbManager();
  PCEFastDbEntry* entry = (PCEFastDbEntry*)dbManager.getEntry();

  // Helpers
  m_emuMenuHelper.selectNode( node );
  m_cartSettingsMenuHelper.selectNode( node );
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
