#ifndef LYNX_MENU_MANAGER_H
#define LYNX_MENU_MANAGER_H

#include "MenuManager.h"
#include "StandardCartSettingsMenuHelper.h"

class LynxMenuManager : public MenuManager
{
private:
  EmulatorMenuHelper m_emuMenuHelper;
  StandardCartSettingsMenuHelper m_cartSettingsMenuHelper;

public:
  LynxMenuManager( Emulator& emulator );
  TREENODE* getEmulatorMenu();
  TREENODE* getCartridgeSettingsMenu();
  void getNodeName( TREENODE* node, char *buffer, char* value );
  void selectNode( TREENODE *node );
  bool isNodeVisible( TREENODE *node );
};

#endif