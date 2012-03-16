#ifndef EMULATOR_MENU_HELPER_H
#define EMULATOR_MENU_HELPER_H

#include "BaseHelper.h"

class EmulatorMenuHelper : public BaseMenuHelper
{
public:
  EmulatorMenuHelper( Emulator& emulator );
  TREENODE* createEmulatorMenu( bool addControls = false );
  void getNodeName( TREENODE* node, char *buffer, char* value );
  void selectNode( TREENODE* node );
  bool isNodeVisible( TREENODE* node );
};

#endif