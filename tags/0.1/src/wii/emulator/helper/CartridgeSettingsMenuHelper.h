#ifndef CARTRIDGE_SETTINGS_MENU_HELPER_H
#define CARTRIDGE_SETTINGS_MENU_HELPER_H

#include "BaseHelper.h"

class CartridgeSettingsMenuHelper : public BaseMenuHelper
{
public:
  CartridgeSettingsMenuHelper( Emulator& emulator );
  TREENODE* createCartridgeSettingsMenu();
  TREENODE* addControlsSettingsNode( TREENODE* parent );
  TREENODE* addControlsMappingsNode( TREENODE* parent );
  TREENODE* addDisplaySettingsNode( TREENODE* parent );
  TREENODE* addAdvancedSettingsNode( TREENODE* parent );
  void addCartSettingsOpsNodes( TREENODE* parent );

  void getNodeName( TREENODE* node, char *buffer, char* value );
  void selectNode( TREENODE* node );
  bool isNodeVisible( TREENODE* node );
};

#endif