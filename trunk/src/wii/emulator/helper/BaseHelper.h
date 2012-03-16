#ifndef BASE_HELPER_H
#define BASE_HELPER_H

#include "Emulator.h"
#include "wii_main.h"

class BaseHelper
{
private:
  Emulator& m_emulator;

public:
  BaseHelper( Emulator& emulator );
  Emulator& getEmulator();
};

class BaseMenuHelper : public BaseHelper
{
public:
  BaseMenuHelper( Emulator& emulator );
  void addSpacerNode( TREENODE* parent );
  virtual void getNodeName( TREENODE* node, char *buffer, char* value ) = 0;
  virtual void selectNode( TREENODE* node ) = 0;
  virtual bool isNodeVisible( TREENODE* node ) = 0;
  const char* getEnabledText( bool isEnabled );
};

#endif