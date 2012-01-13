#include "MenuManager.h"

MenuManager::MenuManager( Emulator& emulator ) : 
  BaseManager( emulator ),
  m_emulatorMenu( NULL ),
  m_cartridgeSettingsMenu( NULL )
{
}

TREENODE* MenuManager::getEmulatorMenu()
{
  return m_emulatorMenu;
}

TREENODE* MenuManager::getCartridgeSettingsMenu()
{
  return m_cartridgeSettingsMenu;
}

void MenuManager::getNodeName( TREENODE* node, char *buffer, char* value )
{
}

void MenuManager::selectNode( TREENODE *node )
{
}

bool MenuManager::isNodeVisible( TREENODE *node )
{
}

