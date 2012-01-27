#ifndef VIRTUALBOY_MENU_MANAGER_H
#define VIRTUALBOY_MENU_MANAGER_H

#include "MenuManager.h"
#include "StandardCartSettingsMenuHelper.h"

class VirtualBoyMenuManager : public MenuManager
{
private:
  EmulatorMenuHelper m_emuMenuHelper;
  StandardCartSettingsMenuHelper m_cartSettingsMenuHelper;

public:
  VirtualBoyMenuManager( Emulator& emulator );
  TREENODE* getEmulatorMenu();
  TREENODE* getCartridgeSettingsMenu();
  void getNodeName( TREENODE* node, char *buffer, char* value );
  void selectNode( TREENODE *node );
  bool isNodeVisible( TREENODE *node );
};

#endif