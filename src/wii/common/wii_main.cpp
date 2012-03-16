/*
Copyright (C) 2011
raz0red (www.twitchasylum.com)

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1.	The origin of this software must not be misrepresented; you
must not claim that you wrote the original software. If you use
this software in a product, an acknowledgment in the product
documentation would be appreciated but is not required.

2.	Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3.	This notice may not be removed or altered from any source
distribution.
*/

#include <stdio.h>
#include <sys/dir.h>

#include <wiiuse/wpad.h>
#include <gccore.h>
#include <ogc/conf.h>
#include <SDL.h>

#include "pngu/pngu.h"
#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"  
#endif

#include "about_png.h"

#ifdef TRACK_UNIQUE_MSGIDS
  extern void dump_unique_msgids();
#endif

#include "wii_gx.h"
#include "wii_main.h"
#include "wii_app.h"
#include "wii_hw_buttons.h"
#include "wii_input.h"
#include "wii_video.h"
#include "wii_sdl.h"
#include "fileop.h"

#include "vi_encoder.h"
#include "gettext.h"

#define ABOUT_Y 20

#define MENU_STARTY     160
#define MENU_HEADERX    30
#define MENU_LINESIZE   20
#define MENU_PAGESIZE   11

extern "C" 
{
Mtx gx_view;
void WII_VideoStop();
void WII_SetDefaultVideoMode();
void WII_SetWidescreen(int wide);
}

// The vsync mode
int wii_vsync = -1;
// The last rom that was successfully executed
char *wii_last_rom = NULL;

// The average FPS from testing vsync 
static float test_fps = 0.0;
// Whether the Wii is PAL or NTSC
BOOL wii_is_pal = 0;
// Whether the wiimote (for the menu) is horizontal or vertical
BOOL wii_mote_menu_vertical = TRUE;
// Whether to return to the top menu after exiting a game
BOOL wii_top_menu_exit = TRUE;
// The stack containing the menus the user has navigated
TREENODEPTR wii_menu_stack[64];
// The head of the menu stack
s8 wii_menu_stack_head = -1;
// The root of the menu
TREENODE *wii_menu_root;
// Whether to quite the menu loop
BOOL wii_menu_quit_loop = 0;
// Forces the menu to be redrawn
BOOL wii_menu_force_redraw = 1;
// The current menu selection
s16 menu_cur_idx = -1;
// Offset of the selection bar in the menu
int wii_menu_sel_offset = 0;
// The menu selection color
RGBA wii_menu_sel_color = { 0, 0, 0xC0, 0 };
// Double strike mode
BOOL wii_double_strike_mode = FALSE;
// Auto widescreen value (from startup)
static BOOL widescreen_auto = FALSE;
// Full widescreen
int wii_full_widescreen = WS_AUTO;
// USB keep alive 
BOOL wii_usb_keepalive = FALSE;
// Trap filter
BOOL wii_trap_filter = FALSE; 
// 16:9 correction
BOOL wii_16_9_correction = FALSE;

// The about image data
static gx_imagedata* about_idata = NULL;
// The first item to display in the menu (paging, etc.)
static s16 menu_start_idx = 0;

// The main args
static int main_argc;
static char **main_argv;

// Forward refs
static void wii_free_node( TREENODE* node );

/*
* Test to see if the machine is PAL or NTSC
*/
static void wii_test_pal()
{
  wii_is_pal = 
    ( VIDEO_GetPreferredMode(NULL)->viTVMode >> 2 ) == VI_PAL;

#ifdef WII_NETTRACE
  net_print_string(NULL,0, "pal test: %d\n", wii_is_pal );  
#endif
}

/*
* Sets whether to enable VSYNC or not 
*
* param    sync Whether to enable VSYNC or not
*/
void wii_set_vsync( BOOL sync )
{
  wii_vsync = ( sync ? VSYNC_ENABLED : VSYNC_DISABLED );
}

/*
* Returns the current menu index
*
* return   The current menu index
*/
s16 wii_menu_get_current_index()
{
  return menu_cur_idx;
}

/*
* Resets the menu indexes when an underlying menu in change (push/pop)
*/
void wii_menu_reset_indexes()
{
  LOCK_RENDER_MUTEX();

  menu_cur_idx = -1;
  menu_start_idx = 0;

  UNLOCK_RENDER_MUTEX();
}

/*
* Creates and returns a new menu tree node
*
* type     The type of the node
* name     The name for the node
* return   The newly created node
*/
TREENODE* wii_create_tree_node( enum NODETYPE type, const char *name )
{
  TREENODE* nodep = (TREENODE*)malloc( sizeof( TREENODE ) );
  memset( nodep, 0, sizeof( TREENODE ) );
  nodep->node_type = type;
  nodep->name = strdup( name );
  nodep->child_count = 0;
  nodep->max_children = 0;

  return nodep;
}

/*
* Attempts to find the tree node with the specified type
*
* root     Where to start the search
* type     The type of the node
* return   The found tree node or NULL
*/
TREENODE* wii_find_tree_node( TREENODE* root, enum NODETYPE type )
{
  TREENODE* ret = NULL;
  for( int i = 0; i < root->child_count && !ret; i++ )
  {
    TREENODE* currChild = root->children[i];
    if( currChild->node_type == type )
    {
      ret = currChild;
    }
    else
    {
      ret = wii_find_tree_node( currChild, type );
    }
  }

  return ret;
}

/*
* Adds the specified child node to the specified parent
*
* parent   The parent
* child    The child to add to the parent
*/
void wii_add_child( TREENODE *parent, TREENODE *childp  )
{
  // Do we have room?
  if( parent->child_count == parent->max_children )
  {
    parent->max_children += 10;
    parent->children = (TREENODEPTR*)
      realloc( parent->children, parent->max_children * 
      sizeof(TREENODEPTR));		
  }

  parent->children[parent->child_count++] = childp;
}

/*
* Clears the children for the specified node
*
* node     The node to clear the children for
*/
void wii_menu_clear_children( TREENODE* node )
{
  LOCK_RENDER_MUTEX();

  int i;
  for( i = 0; i < node->child_count; i++ )
  {
    wii_free_node( node->children[i] );
    node->children[i] = NULL;
  }
  node->child_count = 0;

  UNLOCK_RENDER_MUTEX();
}

/*
* Frees the specified tree node
*
* node     The node to free
*/
static void wii_free_node( TREENODE* node )
{
  wii_menu_clear_children( node );    

  free( node->children );
  free( node->name );	
  free( node );
}

/*
* Pushes the specified menu onto the menu stack (occurs when the user
* navigates to a sub-menu)
*
* menu     The menu to push on the stack
*/
void wii_menu_push( TREENODE *menu )
{    
  LOCK_RENDER_MUTEX();

  wii_menu_stack[++wii_menu_stack_head] = menu;
  wii_menu_reset_indexes();
  wii_menu_move( menu, 1 );

  UNLOCK_RENDER_MUTEX();
}

/*
* Pops the latest menu off of the menu stack (occurs when the user leaves 
* the current menu.
*
* return   The pop'd menu
*/
TREENODE* wii_menu_pop()
{    
  TREENODE* ret = NULL;

  LOCK_RENDER_MUTEX();

  if( wii_menu_stack_head > 0 )
  {
    TREENODE *oldmenu = wii_menu_stack[wii_menu_stack_head--];
    wii_menu_reset_indexes();        
    wii_menu_move( wii_menu_stack[wii_menu_stack_head], 1 );
    ret = oldmenu;
  }

  UNLOCK_RENDER_MUTEX();

  return ret;
}

/*
* Updates the buffer with the header message for the current menu
*
* menu     The menu
* buffer   The buffer to update with the header message for the
*          current menu.
*/
static void wii_menu_get_header( TREENODE* menu, char *buffer )
{
  wii_menu_handle_get_header( menu, buffer );

  if( strlen( buffer ) == 0 )
  {
    snprintf( buffer, WII_MENU_BUFF_SIZE, 
      "%s = %s%s%s, A = %s%s%s, %s = %s", 
      gettextmsg( "U/D" ),
      gettextmsg( "Scroll" ),
      ( menu->child_count > MENU_PAGESIZE ? ", L/R = " : "" ),
      ( menu->child_count > MENU_PAGESIZE ? gettextmsg( "Page" ) : "" ),      
      gettextmsg( "Select" ),
      ( wii_menu_stack_head > 0 ? ", B = " : "" ),
      ( wii_menu_stack_head > 0 ? gettextmsg( "Back" ) : "" ),
      gettextmsg( "Home" ),
      gettextmsg( "Exit" )
    );
  }
}

/*
* Returns the number of visible children in the specified menu
*
* menu     The menu
* return   The number of visible children in the specified menu
*/
static int get_visible_child_count( TREENODE* menu )
{
  int count = 0;
  int i;
  for( i = 0; i < menu->child_count; i++ )
  {
    TREENODE* node = menu->children[i];    
    if( wii_menu_handle_is_node_visible( node ) )
    {
      ++count;
    }
  }
  return count;
}

/*
* Updates the buffer with the footer message for the current menu
*
* menu     The menu
* buffer   The buffer to update with the footer message for the
*          current menu.
*/
static void wii_menu_get_footer( TREENODE* menu, char *buffer )
{    
  buffer[0] = '\0';

  // Any status messages? If so display it in the footer
  if( wii_status_message_count > 0 )
  {        
    wii_status_message_count--;
    snprintf( buffer, WII_MENU_BUFF_SIZE, wii_status_message );
  }
  else
  {
    wii_menu_handle_get_footer( menu, buffer );        

    if( buffer[0] == '\0' )
    {
      int visible = get_visible_child_count( menu );
      if( visible > MENU_PAGESIZE )
      {
        s16 end_idx = menu_start_idx + MENU_PAGESIZE;
        if( end_idx > visible ) end_idx = visible;
        s16 start_idx = ( end_idx - MENU_PAGESIZE ) + 1;
        if( start_idx <= 0 ) start_idx = 1;

        snprintf( buffer, WII_MENU_BUFF_SIZE, 
          "%d %s %d %s %d.",
          visible, 
          gettextmsg( "items found, displaying" ),
          start_idx,
          gettextmsg( "to" ),
          end_idx );                
      }
    }          
  }
}

/*
* Determines whether the node is selectable
*
* node     The node
* return   Whether the node is selectable
*/
static BOOL wii_menu_is_node_selectable( TREENODE *node )
{
  if( !wii_menu_handle_is_node_visible( node ) )
  {
    return FALSE;
  }

  if( node->node_type == NODETYPE_SPACER )
  {
    return FALSE;      
  }

  return wii_menu_handle_is_node_selectable( node );
}

/*
* Renders the specified menu
*/
static void wii_menu_render( TREENODE *menu )
{	
  GXColor headerColor = { 96, 96, 96, 0xff };
  GXColor selColor = { wii_menu_sel_color.R, wii_menu_sel_color.G, wii_menu_sel_color.B, 0xff };
  int fontSize = 18;

  // Render the about image
  wii_gx_drawimage( 
    -(about_idata->width>>1), GX_Y(ABOUT_Y), 
    about_idata->width, about_idata->height, 
    about_idata->data, 
    0, 1.0, 1.0, 0xff );

  // Draw the menu items (text)
  if( menu )
  {	
    char buffer[WII_MENU_BUFF_SIZE];
    char value[WII_MENU_BUFF_SIZE];
    char buffer2[WII_MENU_BUFF_SIZE];

    buffer[0] = '\0';
    wii_menu_get_header( menu, buffer );

    wii_gx_drawtext(
      0, GX_Y( MENU_STARTY ),
      fontSize, (char*)( buffer ), headerColor, FTGX_ALIGN_BOTTOM | FTGX_JUSTIFY_CENTER ); 

    int displayed = 0; 
    int i;
    for( i = menu_start_idx; 
      i < menu->child_count && displayed < MENU_PAGESIZE; 
      i++ )
    {		
      buffer[0] = '\0';
      value[0] = '\0';

      TREENODE* node = menu->children[i];
      if( wii_menu_handle_is_node_visible( node ) )
      {
        wii_menu_handle_get_node_name( node, buffer, value );

        if( menu_cur_idx == i )
        {
          wii_gx_drawrectangle( 
            GX_X( 0 ),
            GX_Y( MENU_STARTY + ( ( displayed + 1 ) * MENU_LINESIZE ) + 1 /*+ wii_menu_sel_offset*/ ), 
            640, 21,
            selColor, 1);
        }

        BOOL hasValue = value[0] != '\0';
        
        snprintf( buffer2, WII_MENU_BUFF_SIZE, "%s%s", 
          ( node->node_type != NODETYPE_ROM ?
              gettextmsg( buffer ) :
              buffer ), 
          hasValue ? " " : "" );
    
        wii_gx_drawtext(
          0, GX_Y( MENU_STARTY + ( ( displayed + 2 ) * MENU_LINESIZE ) ),
          fontSize, buffer2, ftgxWhite, FTGX_ALIGN_BOTTOM | 
            ( hasValue ? FTGX_JUSTIFY_RIGHT : FTGX_JUSTIFY_CENTER ) ); 

        if( hasValue )
        {
          snprintf( buffer2, WII_MENU_BUFF_SIZE, ": %s", gettextmsg( value ) );

          wii_gx_drawtext(
            0, GX_Y( MENU_STARTY + ( ( displayed + 2 ) * MENU_LINESIZE ) ),
            fontSize, buffer2, ftgxWhite, FTGX_ALIGN_BOTTOM | FTGX_JUSTIFY_LEFT ); 
        }

        ++displayed;
      }
    }

    buffer[0] = '\0';
    wii_menu_get_footer( menu, buffer );

    wii_gx_drawtext(
      0, GX_Y( MENU_STARTY + ( ( MENU_PAGESIZE + 3 ) * MENU_LINESIZE ) ),
      fontSize, buffer, headerColor, FTGX_ALIGN_BOTTOM | FTGX_JUSTIFY_CENTER ); 
  }
}

/*
* Returns the index to the start of the page from the currently selected
* index.
*
* return   The index to the start of the page from the currently selected
*          index.
*/
static s16 get_page_start_idx( TREENODE *menu )
{
  int count = MENU_PAGESIZE - 1;
  s16 index = menu_cur_idx;
  while( count > 0 )
  {
    if( --index < 0 ) return 0;

    TREENODE* node = menu->children[index];
    if( wii_menu_handle_is_node_visible( node ) )
    {
      count--;
    }
  }

  return index;
}

/*
* Move the current selection in the menu by the specified step count
*
* menu     The current menu
* steps    The number of steps to move the selection
*/
void wii_menu_move( TREENODE *menu, s16 steps )
{	
  if( !menu ) return;

  LOCK_RENDER_MUTEX();

  s16 new_idx = menu_cur_idx;

  //
  // Attempt to move to the new location
  //

  while( 1 )
  {
    s16 prev_idx = new_idx;

    //
    // Move the requested number of steps, skipping items that
    // are not visible
    //
    int stepCount = steps < 0 ? -steps : steps;
    int stepSize = steps < 0 ? -1 : 1;
    while( stepCount > 0 )
    { 
      new_idx += stepSize;

      if( ( new_idx >= menu->child_count ) ||
        ( new_idx < 0 ) )
      {
        break;
      }

      TREENODE* curchild = menu->children[ new_idx ];
      if( wii_menu_handle_is_node_visible( curchild ) )
      {
        stepCount -= 1;
      }
    }    

    //
    // Handle edges
    //
    if( new_idx >= menu->child_count )
    {
      new_idx = menu->child_count - 1;
    }		

    if( new_idx < 0 )
    {
      new_idx = 0;
    }

    //
    // Make sure the item we ended up on can be selected
    //
    if( new_idx < menu->child_count )
    {
      TREENODE* curchild = menu->children[new_idx];
      if( ( !wii_menu_is_node_selectable( curchild ) ) &&
        prev_idx != new_idx )
      {
        steps = steps > 0 ? 1 : -1;
      }
      else
      {
        break;
      } 
    }
    else
    {
      break;
    }
  }

  //
  // Is the new location valid? If so make updates.
  //
  if( new_idx >= 0 && 
    new_idx < menu->child_count &&
    wii_menu_is_node_selectable( menu->children[new_idx] ) )
  {
    menu_cur_idx = new_idx;

    if( menu_cur_idx < menu_start_idx )
    {
      menu_start_idx = menu_cur_idx;
    }
    else
    {
      s16 min_start = get_page_start_idx( menu );
      if( min_start > menu_start_idx )
      {
        menu_start_idx = min_start;
      }
    }
  }

  UNLOCK_RENDER_MUTEX();
}

/*
 * The callback used to render the menu
 */
static void menu_render_callback()
{  
  GX_SetVtxDesc( GX_VA_POS, GX_DIRECT );
  GX_SetVtxDesc( GX_VA_CLR0, GX_DIRECT );
  GX_SetVtxDesc( GX_VA_TEX0, GX_NONE );

  Mtx m;    // model matrix.
  Mtx mv;   // modelview matrix.

  guMtxIdentity( m ); 
  guMtxTransApply( m, m, 0, 0, -100 );
  guMtxConcat( gx_view, m, mv );
  GX_LoadPosMtxImm( mv, GX_PNMTX0 ); 

  LOCK_RENDER_MUTEX();

  TREENODE *menu = 
    wii_menu_stack_head >= 0 ?			
    wii_menu_stack[wii_menu_stack_head] : NULL;

  if( menu != NULL )
  {    
    wii_menu_render( menu );    
  }

  UNLOCK_RENDER_MUTEX();
}

#define DELAY_FRAMES 6
#define DELAY_STEP 1
#define DELAY_MIN 2

static void precallback()
{
  if( wii_usb_keepalive )
  {
    UsbKeepAlive(); // Attempt to keep the USB drive from sleeping...
  }

  static BOOL lastws = 0;
  int newws = 
    ( wii_full_widescreen == WS_AUTO ? 
        widescreen_auto : wii_full_widescreen );

  if( lastws != newws )
  {
    lastws = newws;
    WII_SetWidescreen( lastws );
  }

  VIDEO_SetTrapFilter( 1 );
  WII_SetDefaultVideoMode();
}

/*
* Displays the menu 
*/
void wii_menu_show()
{
  // Push our callback
  wii_gx_push_callback( &menu_render_callback, FALSE, &precallback );

  // Allows for incremental speed when scrolling the menu 
  // (Scrolls faster the longer the directional pad is held)
  s16 delay_frames = -1;
  s16 delay_factor = -1;

  // Invoke the menu pre loop handler
  wii_menu_handle_pre_loop();

  wii_menu_quit_loop = 0;

  while( !wii_menu_quit_loop )
  {		
    // Scan the Wii and Gamecube controllers
    WPAD_ScanPads();
    PAD_ScanPads();        

    // Check the state of the controllers
    u32 down = WPAD_ButtonsDown( 0 );
    u32 held = WPAD_ButtonsHeld( 0 );
    u32 gcDown = PAD_ButtonsDown( 0 );
    u32 gcHeld = PAD_ButtonsHeld( 0 );

    if( ( down & WII_BUTTON_HOME ) ||
      ( gcDown & GC_BUTTON_HOME ) || 
      wii_hw_button )
    {
      // Handle the home button being pressed
      wii_menu_handle_home_button();

      wii_menu_quit_loop = 1;
      continue;
    }

    // Analog controls
    expansion_t exp;
    WPAD_Expansion( 0, &exp );        
    float expX = wii_exp_analog_val( &exp, TRUE, FALSE );
    float expY = wii_exp_analog_val( &exp, FALSE, FALSE );
    s8 gcX = PAD_StickX( 0 );
    s8 gcY = PAD_StickY( 0 );

    // Classic or Nunchuck?
    bool isClassic = ( exp.type == WPAD_EXP_CLASSIC );

    TREENODE *menu = 
      wii_menu_stack_head >= 0 ?			
      wii_menu_stack[wii_menu_stack_head] : NULL;

    if( menu )
    {                    
      wii_menu_handle_update( menu );

      if( ( ( held & (
        WII_BUTTON_LEFT | WII_BUTTON_RIGHT | 
        WII_BUTTON_DOWN | WII_BUTTON_UP |
        ( isClassic ? 
        ( WII_CLASSIC_BUTTON_LEFT | WII_CLASSIC_BUTTON_UP ) : 0 ) 
        ) ) == 0 ) &&
        ( ( gcHeld & (
        GC_BUTTON_LEFT | GC_BUTTON_RIGHT |
        GC_BUTTON_DOWN | GC_BUTTON_UP ) ) == 0 ) &&
        ( !wii_analog_left( expX, gcX ) &&
        !wii_analog_right( expX, gcX ) &&
        !wii_analog_up( expY, gcY )&&
        !wii_analog_down( expY, gcY ) ) )
      {
        delay_frames = -1;
        delay_factor = -1;
      }
      else
      {
        if( delay_frames < 0 )
        {
          if( 
            wii_digital_left( !wii_mote_menu_vertical, isClassic, held ) || 
            ( gcHeld & GC_BUTTON_LEFT ) ||                       
            wii_analog_left( expX, gcX ) )
          {
            wii_menu_move( menu, -MENU_PAGESIZE );
            wii_menu_force_redraw = 1;
          }
          else if( 
            wii_digital_right( !wii_mote_menu_vertical, isClassic, held ) ||
            ( gcHeld & GC_BUTTON_RIGHT ) ||
            wii_analog_right( expX, gcX ) )
          {
            wii_menu_move( menu, MENU_PAGESIZE );
            wii_menu_force_redraw = 1;
          }
          else if( 
            wii_digital_down( !wii_mote_menu_vertical, isClassic, held ) ||
            ( gcHeld & GC_BUTTON_DOWN ) ||
            wii_analog_down( expY, gcY ) )
          {
            wii_menu_move( menu, 1 );
            wii_menu_force_redraw = 1;
          }
          else if( 
            wii_digital_up( !wii_mote_menu_vertical, isClassic, held ) || 
            ( gcHeld & GC_BUTTON_UP ) ||
            wii_analog_up( expY, gcY ) )
          {
            if( menu )
            {
              wii_menu_move( menu, -1 );						
              wii_menu_force_redraw = 1;
            }
          }

          delay_frames = 
            DELAY_FRAMES - (DELAY_STEP * ++delay_factor);

          if( delay_frames < DELAY_MIN ) 
          {
            delay_frames = DELAY_MIN;
          }
        }
        else
        {
          delay_frames--;
        }
      }

      if( ( ( down & ( WII_BUTTON_A | 
            ( isClassic ? WII_CLASSIC_BUTTON_A : WII_NUNCHUK_BUTTON_A ) ) ) ||
            ( gcDown & GC_BUTTON_A ) ) &&
            menu_cur_idx != -1 )
      {	
        wii_menu_handle_select_node( menu->children[menu_cur_idx] );
        wii_menu_force_redraw = 1;            
      }
      if( ( down & ( WII_BUTTON_B | 
        ( isClassic ? WII_CLASSIC_BUTTON_B : WII_NUNCHUK_BUTTON_B ) ) ) || 
        ( gcDown & GC_BUTTON_B ) )
      {
        wii_menu_pop();
        wii_menu_force_redraw = 1;
      }
    }

    VIDEO_WaitVSync();
  }

#ifdef TRACK_UNIQUE_MSGIDS
  dump_unique_msgids();
#endif

  // Pop our callback
  wii_gx_pop_callback();

  // Invoke post loop handler
  wii_menu_handle_post_loop();
}

/*
* Used for comparing menu names when sorting (qsort)
*
* a        The first tree node to compare
* b        The second tree node to compare
* return   The result of the comparison
*/
int wii_menu_name_compare( const void *a, const void *b )
{
  TREENODE** aptr = (TREENODE**)a;
  TREENODE** bptr = (TREENODE**)b;

  return stricmp( (*aptr)->name, (*bptr)->name );
}

/*
* Returns the standard list footer for snapshots, games, etc.
*
* menu     The current menu
* listname The name of the list (snapshot/game, etc.)
* listname The name of the list in plural form (snapshot/game, etc.)
* buffer   The buffer to write the footer to
*/
void wii_get_list_footer( 
  TREENODE* menu, 
  const char *listname, 
  const char *listnameplural, 
  char *buffer )
{
  char buffer2[WII_MENU_BUFF_SIZE] = "";
  if( menu->child_count == 0 )
  {
    snprintf( 
      buffer2, WII_MENU_BUFF_SIZE, "No %s found.", listnameplural );
 
    snprintf( 
      buffer, WII_MENU_BUFF_SIZE, "%s", gettextmsg( buffer2 ) );                  
  }
  else if( menu->child_count == 1 )
  {
    snprintf( 
      buffer2, WII_MENU_BUFF_SIZE, "1 %s found.", listname );

    snprintf( 
      buffer, WII_MENU_BUFF_SIZE, "%s", gettextmsg( buffer2 ) );                
  }
  else
  {
    snprintf( 
      buffer2, WII_MENU_BUFF_SIZE, "%s found, displaying", listnameplural );

    s16 end_idx = menu_start_idx + MENU_PAGESIZE;
    snprintf( buffer, WII_MENU_BUFF_SIZE, 
      "%d %s %d %s %d.", menu->child_count, gettextmsg( buffer2 ),
      menu_start_idx + 1, gettextmsg( "to" ),
      ( end_idx < menu->child_count ? end_idx : menu->child_count )                            
    );                
  }
}

/*
* Loads the resource for the menu
*/
static void init_app()
{
  // Load the about image
  about_idata = wii_gx_loadimagefrombuff( about_png );

  // Initialize the application
  wii_handle_init();
}

/*
* Frees the menu resources 
*/
static void free_resources()
{
  WII_VideoStop();

  //
  // Probably completely unnecessary but seems like a good time
  //

#if 0
  if( about_buff != NULL )
  {
    free( about_buff );
    about_buff = NULL;
  }
#endif

  if( wii_menu_root != NULL )
  {
    wii_free_node( wii_menu_root );
    wii_menu_root = NULL;
  }

  if( wii_last_rom != NULL )
  {
    free( wii_last_rom );
    wii_last_rom = NULL;
  }

  // Free application resources
  wii_handle_free_resources();
}

/*
* Flush and syncs the video 
*/
void wii_sync_video()
{     
  if( wii_vsync_enabled() )
  {
    VIDEO_WaitVSync();
  }
}

/*
* Returns whether VSYNC is enabled
*
* return Whether VSYNC is enabled
*/
int wii_vsync_enabled()
{
  return 
    ( wii_vsync == VSYNC_ENABLED ) && 
    ( ( !wii_is_pal && wii_get_max_frames() <= 60 ) ||
    ( wii_is_pal && wii_get_max_frames() <= 50 ) );
}

#ifdef WII_NETTRACE
static int __out_write(struct _reent *r, int fd, const char *ptr, size_t len) 
{         
  if (!ptr || len <= 0)
  {
    return -1;
  }
  char message[512];
  Util_strlcpy( message, ptr, sizeof(message) < len ? sizeof(message) : len ); 
  net_print_string( NULL, 0, "%s\n", message );      
  return len;
}

const devoptab_t dot_out = {
  "stdout", // device name
  0, // size of file structure
  NULL, // device open
  NULL, // device close
  __out_write,// device write
  NULL, // device read
  NULL, // device seek
  NULL, // device fstat
  NULL, // device stat
  NULL, // device link
  NULL, // device unlink
  NULL, // device chdir
  NULL, // device rename
  NULL, // device mkdir
  0, // dirStateSize
  NULL, // device diropen_r
  NULL, // device dirreset_r
  NULL, // device dirnext_r
  NULL, // device dirclose_r
  NULL // device statvfs_r
};
#endif

/*
* Main 
*/
int main(int argc,char *argv[])
{
#ifdef WII_NETTRACE
  char localip[16] = {0};
  char gateway[16] = {0};
  char netmask[16] = {0};	
  if_config ( localip, netmask, gateway, TRUE);

  // First arg represents IP address for remote tracing
  net_print_init( ( argc > 1 ? argv[1] : NULL ), 0 );

  int i;
  for( i = 0; i < argc; i++ )
  {
    char val[256];
    snprintf( val, sizeof( val ), "arg[%d]: %s\n", i, argv[i] );
    net_print_string(__FILE__,__LINE__, val );
  }

  devoptab_list[STD_OUT] = &dot_out;
  devoptab_list[STD_ERR] = &dot_out;
#endif

  //printf( "\x1b[5;0H" );

  main_argc = argc;
  main_argv = argv;

  // Initialize the Wii
  wii_set_app_path( argc, argv );  
    
  // Try to mount the file system
  if( !ChangeInterface( wii_get_app_path(), FS_RETRY_COUNT ) ) 
  {
    CON_Init(xfb[0],20,20,vmode->fbWidth,vmode->xfbHeight,vmode->fbWidth*VI_DISPLAY_PIX_SZ);
    printf( "\x1b[5;0H" );
    printf( "Unable to mount %s\n\n", wii_get_fs_prefix() );
    printf( "Press A to exit..." );
    wii_pause();
#ifdef WII_NETTRACE
    net_print_close();
#endif
    exit( 0 ); // unable to mount file system
  }

  WPAD_Init();
  PAD_Init();

  // Set the hardware callbacks
  wii_register_hw_buttons();

  // Clear the stack
  memset( &wii_menu_stack, 0, sizeof(wii_menu_stack) );

  // Initializes the application
  init_app(); 

  // Test for PAL/NTSC
  wii_test_pal();

  if( wii_vsync == -1 )
  {
    // Set the vsync based on whether or not we are PAL or NTSC
    wii_set_vsync( !wii_is_pal );
  }

  // Determine widescreen auto value
  widescreen_auto = ( CONF_GetAspectRatio() == CONF_ASPECT_16_9 );

  // Runs the application
  wii_handle_run();

  // Frees the application resources
  free_resources();    

#ifdef WII_NETTRACE
  net_print_close();
#endif

  if( wii_hw_button )
  {
    // They pressed a HW button, reset based on its type
    SYS_ResetSystem( wii_hw_button, 0, 0 );
  }
  else if( !!*(u32*)0x80001800 ) 
  {
    // Were we launched via HBC?
    exit(1);
  }
  else
  {
    // Wii channel support
    SYS_ResetSystem( SYS_RETURNTOMENU, 0, 0 );
  }

  return 0;
}
