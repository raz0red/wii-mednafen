#include "EmulatorMenuHelper.h"
#include "wii_app.h"
#include "wii_gx.h"
#include "wii_sdl.h"
#include "wii_mednafen.h"
#include "wii_mednafen_resize_screen.h"
#include "gettext.h"

EmulatorMenuHelper::EmulatorMenuHelper( Emulator& emulator ) : 
  BaseMenuHelper( emulator )
{
}

TREENODE* EmulatorMenuHelper::createEmulatorMenu( bool addControls )
{
  TREENODE* menu = wii_create_tree_node( NODETYPE_EMULATOR_SETTINGS, "" );  
  TREENODE* child = wii_create_tree_node( NODETYPE_RESIZE_SCREEN, 
    "Screen size" );      
  wii_add_child( menu, child ); 

  addSpacerNode(  menu );

  if( addControls )
  {
    TREENODE *controls = wii_create_tree_node( 
      NODETYPE_CONTROLS_SETTINGS, "Control settings" );                                                        
    wii_add_child( menu, controls );

    addSpacerNode( menu );
  }

  child = wii_create_tree_node( NODETYPE_VOLUME_CART, "Volume" );
    wii_add_child( menu, child );

  Emulator& emu = getEmulator();
  if( emu.isDoubleStrikeSupported() )
  {    
    child = wii_create_tree_node( 
      NODETYPE_DOUBLE_STRIKE_CART, "Double strike (240p)" );
    wii_add_child( menu, child );
  }

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
    case NODETYPE_VOLUME_CART:
      {
        int volume = emu.getVolume();
        if( volume == VOLUME_DEFAULT )
        {
          snprintf( value, WII_MENU_BUFF_SIZE, "(%d, %s)", 
            wii_volume / 10, gettextmsg( "global" ) ); 
        }
        else
        {
          snprintf( value, WII_MENU_BUFF_SIZE, "%d %s", 
            ( volume / 10 ), 
            ( volume == 100 ?  gettextmsg("(normal)") : "" ) );
        }
      }
      break;
    case NODETYPE_DOUBLE_STRIKE_CART:
      {
        switch( emu.getDoubleStrikeMode() )
        {
          case DOUBLE_STRIKE_DEFAULT:
            snprintf( value, WII_MENU_BUFF_SIZE, "(%s, %s)", 
              gettextmsg( getEnabledText( wii_double_strike_mode ) ),
              gettextmsg( "global" ) );
            break;
          default:
             snprintf( value, WII_MENU_BUFF_SIZE, "%s", 
               getEnabledText( 
                  emu.getDoubleStrikeMode() == DOUBLE_STRIKE_ENABLED ) );
            break;
        }
      }
      break;
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
    case NODETYPE_CONTROLS_SETTINGS:
      wii_menu_push( node );
      break;
    case NODETYPE_VOLUME_CART:
      {
        int volume = emu.getVolume();
        volume += 10;
        if( volume > 160 )
        {
          volume = 0;
        }
        emu.setVolume( volume );
      }
      break;
    case NODETYPE_DOUBLE_STRIKE_CART:
      {
        int mode = emu.getDoubleStrikeMode();
        mode++;
        if( mode > DOUBLE_STRIKE_DEFAULT )
        {
          mode = 0;
        }
        emu.setDoubleStrikeMode( mode );
      }
      break;
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