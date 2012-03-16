#ifndef STANDARD_CART_SETTINGS_MENU_HELPER_H
#define STANDARD_CART_SETTINGS_MENU_HELPER_H

#include "CartridgeSettingsMenuHelper.h"

class StandardCartSettingsMenuHelper : public CartridgeSettingsMenuHelper
{
private:
  int m_currentController;
  int m_currentProfile;

public:
  StandardCartSettingsMenuHelper( Emulator& emulator );
  void addProfileNode( TREENODE* parent );
  void addControllerNode( TREENODE* parent );
  void addWiimoteSupportedNode( TREENODE* parent );
  void addButtonMappingNodes( TREENODE* parent );
  void getNodeName( TREENODE* node, char *buffer, char* value );
  void selectNode( TREENODE* node );
  void setCurrentProfile( int profile );
  bool isNodeVisible( TREENODE* node );
};

#endif