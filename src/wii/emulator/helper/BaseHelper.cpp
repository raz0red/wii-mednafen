#include "BaseHelper.h"

BaseHelper::BaseHelper( Emulator& emulator ) : m_emulator( emulator )
{
}

Emulator& BaseHelper::getEmulator()
{
  return m_emulator;
}

BaseMenuHelper::BaseMenuHelper( Emulator& emulator ) : BaseHelper( emulator )
{
}

void BaseMenuHelper::addSpacerNode( TREENODE* parent )
{
  TREENODE* child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( parent, child );
}

const char* BaseMenuHelper::getEnabledText( bool isEnabled )
{
  return isEnabled ? "Enabled" : "Disabled";
}
