#include "EmulatorMenuHelper.h"
#include "wii_app.h"
#include "wii_gx.h"
#include "wii_sdl.h"
#include "wii_mednafen_resize_screen.h"
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

  child = wii_create_tree_node( NODETYPE_FRAME_SKIP, "Frame skip" );
  wii_add_child( menu, child );

  return menu;
}

void EmulatorMenuHelper::getNodeName( 
  TREENODE* node, char *buffer, char* value )
{ 
  Emulator& emu = getEmulator();

  switch( node->node_type )
  {
    case NODETYPE_FRAME_SKIP:
      snprintf( value, WII_MENU_BUFF_SIZE, 
        emu.getFrameSkip() ? "Enabled" : "Disabled" );
      break;
    case NODETYPE_RESIZE_SCREEN:
      {
        if( emu.isDoubleStrikeEnabled() )
        {
          snprintf( value, WII_MENU_BUFF_SIZE, "%s",
            "Fixed (double strike)" );
        }
        else
        {
          if( emu.isRotationSupported() )
          {
            snprintf( buffer, WII_MENU_BUFF_SIZE, "%s (%s)",
              gettextmsg(node->name), 
              gettextmsg(
                ( emu.getRotation() ? "Vert." : "Horiz." ) ) );               
          }

          snprintf( value, WII_MENU_BUFF_SIZE, "%s",
            emu.getRotation() ? 
              emu.getRotatedScreenSizeName() : emu.getScreenSizeName() );
        }
      }
      break;    
  }
}

void EmulatorMenuHelper::selectNode( TREENODE* node )
{
  Emulator& emu = getEmulator();

  if( node->node_type == NODETYPE_RESIZE_SCREEN )
  { 
    if( emu.isDoubleStrikeEnabled() )
    {
      wii_set_status_message( 
        "Resizing not supported for double strike mode." );
    }
    else
    {
      // Essentially blanks the screen
      wii_gx_push_callback( NULL, FALSE, NULL );

      wii_sdl_flip(); 
   
      Rect* screenSize = 
        emu.getRotation() ?
          emu.getRotatedScreenSize() : emu.getScreenSize();

      resize_info rinfo = 
      { 
        screenSize->w, 
        screenSize->h, 
        emu.getRotation(),
        emu 
      };

      wii_resize_screen_gui( &rinfo );

      screenSize->w = ( rinfo.currentX );
      screenSize->h = ( rinfo.currentY );

      wii_gx_pop_callback();
    }
  }

  LOCK_RENDER_MUTEX();

  switch( node->node_type )
  {
    case NODETYPE_FRAME_SKIP:
      emu.setFrameSkip( !emu.getFrameSkip() );
      break;
  }

  UNLOCK_RENDER_MUTEX();
}

bool EmulatorMenuHelper::isNodeVisible( TREENODE* node )
{
  return true;
}