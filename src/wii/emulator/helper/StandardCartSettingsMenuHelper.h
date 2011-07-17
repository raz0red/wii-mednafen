#ifndef STANDARD_CART_SETTINGS_MENU_HELPER_H
#define STANDARD_CART_SETTINGS_MENU_HELPER_H

#include "CartridgeSettingsMenuHelper.h"

class StandardCartSettingsMenuHelper : public CartridgeSettingsMenuHelper
{
private:
  int m_currentController;

public:
  StandardCartSettingsMenuHelper( Emulator& emulator );
  void addControllerNode( TREENODE* parent );
  void addWiimoteSupportedNode( TREENODE* parent );
  void addButtonMappingNodes( TREENODE* parent );
  void getNodeName( TREENODE* node, char *buffer, char* value );
  void selectNode( TREENODE* node );
  bool isNodeVisible( TREENODE* node );
};

#endif