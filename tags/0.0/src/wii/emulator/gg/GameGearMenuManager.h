#ifndef GAMEGEAR_MENU_MANAGER_H
#define GAMEGEAR_MENU_MANAGER_H

#include "MenuManager.h"
#include "StandardCartSettingsMenuHelper.h"

class GameGearMenuManager : public MenuManager
{
private:
  EmulatorMenuHelper m_emuMenuHelper;
  StandardCartSettingsMenuHelper m_cartSettingsMenuHelper;

public:
  GameGearMenuManager( Emulator& emulator );
  TREENODE* getEmulatorMenu();
  TREENODE* getCartridgeSettingsMenu();
  void getNodeName( TREENODE* node, char *buffer, char* value );
  void selectNode( TREENODE *node );
  bool isNodeVisible( TREENODE *node );
};

#endif