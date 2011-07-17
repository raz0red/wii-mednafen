#include "EmulatorMenuHelper.h"
#include "wii_gx.h"
#include "wii_sdl.h"
#include "wii_resize_screen.h"
#include "gettext.h"

EmulatorMenuHelper::EmulatorMenuHelper( Emulator& emulator ) : 
  BaseMenuHelper( emulator )
{
}

TREENODE* EmulatorMenuHelper::createEmulatorMenu()
{
  TREENODE* menu = wii_create_tree_node( NODETYPE_EMULATOR_SETTINGS, "" );  
  TREENODE* child = wii_create_tree_node( NODETYPE_RESIZE_SCREEN, 
    "Screen size" );      
  wii_add_child( menu, child ); 

  return menu;
}

void EmulatorMenuHelper::getNodeName( 
  TREENODE* node, char *buffer, char* value )
{
  Emulator& emu = getEmulator();

  switch( node->node_type )
  {
    case NODETYPE_RESIZE_SCREEN:
      {
        if( emu.isRotationSupported() )
        {
          snprintf( buffer, WII_MENU_BUFF_SIZE, "%s (%s)",
            gettextmsg(node->name), 
            gettextmsg(
              ( emu.getRotation() ? "Vert." : "Horiz." ) ) );               
        }

        Rect* defaultScreenSize =
          emu.getRotation() ? 
            emu.getDefaultRotatedScreenSize() : emu.getDefaultScreenSize();
        Rect* screenSize = 
          emu.getRotation() ?
            emu.getRotatedScreenSize() : emu.getScreenSize();

        snprintf( value, WII_MENU_BUFF_SIZE, "%s", 
          ( ( screenSize->w == defaultScreenSize->w &&
              screenSize->h == defaultScreenSize->h ) ? 
                "(default)" : "Custom" ) );
      }
      break;    
  }
}

void EmulatorMenuHelper::selectNode( TREENODE* node )
{
  Emulator& emu = getEmulator();

  if( node->node_type == NODETYPE_RESIZE_SCREEN )
  {   
    // Essentially blanks the screen
    wii_gx_push_callback( NULL, FALSE );

    wii_resize_screen_draw_border( back_surface, 0, back_surface->h );
    wii_sdl_flip(); 

    Rect* defaultScreenSize =
      emu.getRotation() ? 
        emu.getDefaultRotatedScreenSize() : emu.getDefaultScreenSize();
    Rect* screenSize = 
      emu.getRotation() ?
        emu.getRotatedScreenSize() : emu.getScreenSize();

    resize_info rinfo = 
      { defaultScreenSize->w, defaultScreenSize->h, 
        screenSize->w, screenSize->h };
    wii_resize_screen_gui( &rinfo );
    screenSize->w = rinfo.currentX;
    screenSize->h = rinfo.currentY;

    wii_gx_pop_callback();
  }
}

bool EmulatorMenuHelper::isNodeVisible( TREENODE* node )
{
  return true;
}