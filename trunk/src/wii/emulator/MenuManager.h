#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include "Emulator.h"
#include "BaseManager.h"
#include "EmulatorMenuHelper.h"
#include "CartridgeSettingsMenuHelper.h"
#include "wii_main.h"

class MenuManager : public BaseManager
{
protected:
  TREENODE* m_emulatorMenu;
  TREENODE* m_cartridgeSettingsMenu;

public:
  MenuManager( Emulator& emulator );  
  TREENODE* getEmulatorMenu();
  TREENODE* getCartridgeSettingsMenu();
  virtual void getNodeName( TREENODE* node, char *buffer, char* value );
  virtual void selectNode( TREENODE *node );
  virtual bool isNodeVisible( TREENODE *node );
};

#endif