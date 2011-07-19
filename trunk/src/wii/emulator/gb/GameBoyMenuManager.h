#ifndef GAMEBOY_MENU_MANAGER_H
#define GAMEBOY_MENU_MANAGER_H

#include "MenuManager.h"
#include "StandardCartSettingsMenuHelper.h"

class GameBoyMenuManager : public MenuManager
{
private:
  EmulatorMenuHelper m_emuMenuHelper;
  StandardCartSettingsMenuHelper m_cartSettingsMenuHelper;

public:
  GameBoyMenuManager( Emulator& emulator );
  TREENODE* getEmulatorMenu();
  TREENODE* getCartridgeSettingsMenu();
  void getNodeName( TREENODE* node, char *buffer, char* value );
  void selectNode( TREENODE *node );
  bool isNodeVisible( TREENODE *node );
};

#endif