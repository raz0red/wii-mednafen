/*
WiiMednafen : Wii port of the Mednafen emulator

Copyright (C) 2011
raz0red and Arikado

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
#include <stdlib.h>

#include "wii_app.h"
#include "wii_resize_screen.h"
#include "wii_sdl.h"
#include "wii_snapshot.h"
#include "wii_util.h"
#include "wii_gx.h"

#include "wii_mednafen.h"
#include "wii_mednafen_emulation.h"
#include "wii_mednafen_sdl.h"
#include "wii_mednafen_snapshot.h"

#include "Emulators.h"

#include "gettext.h"

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"  
#endif

// Have we read the games list yet?
static BOOL games_read = FALSE;
// Have we read the save state list yet?
static BOOL save_states_read = FALSE;
// The index of the last rom that was run
static s16 last_rom_index = 1;
// Whether the save exists
static BOOL saveexists = FALSE;
// The last save name
static char lastsavename[WII_MAX_PATH] = "";
// The language menu
static TREENODE *language_menu;
// The current language index
static u8 language_index = 0;

// Forward refs
static void wii_read_save_state_list( TREENODE *menu );
static void wii_read_game_list( TREENODE *menu );
static void read_lang_list( TREENODE *menu );

/*
 * Saves the current games state to the specified save file
 *
 * savefile The name of the save file to write state to. If this value is NULL,
 *          the default save name for the last rom is used.
 */
static void save_snapshot( const char *savefile, BOOL status_update )
{
  wii_save_snapshot( savefile, status_update );
  lastsavename[0] = '\0'; // Force recheck of save state
}

/*
 * Loads the currently selected language
 */
static void select_language()
{
  TREENODE* lang = language_menu->children[language_index];
  if( lang->node_type == NODETYPE_LANG_DEFAULT )
  {
    wii_language[0] = '\0'; // update selected language 
    LoadLanguage( NULL );
  }
  else
  {
    // update selected language 
    Util_strlcpy( wii_language, lang->name, sizeof(wii_language) );

    char langfile[WII_MAX_PATH] = "";
    snprintf( langfile, WII_MAX_PATH, "%s%s.%s", 
      wii_get_lang_dir(), lang->name, WII_LANG_EXT );    
    LoadLanguage( langfile );    
  }
}

/*
 * Initializes the menu
 */
void wii_mednafen_menu_init()
{ 
  TREENODE* child = NULL;

  // 
  // Languages menu
  //

  language_menu = wii_create_tree_node( NODETYPE_LANG, "languages" );  
  child = wii_create_tree_node( NODETYPE_LANG_DEFAULT, "English" );
  wii_add_child( language_menu, child );  
  read_lang_list( language_menu );

  // Sync the language with the menu index
  for( int i = 0; i < language_menu->child_count; i++ )
  {
    TREENODE* node = language_menu->children[i];
    if( node->node_type == NODETYPE_LANG_DEFAULT )
    {
      // Set the default index
      language_index = i;
    }
    else if( !strcmp( node->name, wii_language ) )
    {
      // Found the language.. exit the loop
      language_index = i; 
      break;
    }
  }

  select_language(); // Load the selected language  

  //
  // The root menu
  //

  wii_menu_root = wii_create_tree_node( NODETYPE_ROOT, "root" );  

  child = wii_create_tree_node( NODETYPE_RESUME, "Resume" );
  wii_add_child( wii_menu_root, child );
  
  child = wii_create_tree_node( NODETYPE_RESET, "Reset" );
  wii_add_child( wii_menu_root, child );

  child = wii_create_tree_node( NODETYPE_LOAD_ROM, "Load game" );
  wii_add_child( wii_menu_root, child );

  child = wii_create_tree_node( NODETYPE_CARTRIDGE_SETTINGS_CURRENT_SPACER, "" );
  wii_add_child( wii_menu_root, child );

  TREENODE *cart_settings = wii_create_tree_node( 
    NODETYPE_CARTRIDGE_SETTINGS_CURRENT, "Cartridge settings (current cartridge)" );
  wii_add_child( wii_menu_root, cart_settings );    

  TREENODE *emulator_settings = wii_create_tree_node( 
    NODETYPE_EMULATOR_SETTINGS, "Emulator settings" );                                                        
  wii_add_child( wii_menu_root, emulator_settings );

  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( wii_menu_root, child );

  //
  // Save state management
  //

  TREENODE *state = wii_create_tree_node( 
    NODETYPE_SAVE_STATE_MANAGEMENT, "Save state management" );
  wii_add_child( wii_menu_root, state );

  child = wii_create_tree_node( NODETYPE_AUTO_LOAD_STATE, 
    "Auto load" );
  wii_add_child( state, child );

  child = wii_create_tree_node( NODETYPE_AUTO_SAVE_STATE, 
    "Auto save" );
  wii_add_child( state, child );

  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( state, child );

  child = wii_create_tree_node( NODETYPE_LOAD_STATE, 
    "Load saved state" );
  wii_add_child( state, child );

  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( state, child );

  child = wii_create_tree_node( NODETYPE_SAVE_STATE, 
    "Save state (current cartridge)" );
  wii_add_child( state, child );

  child = wii_create_tree_node( NODETYPE_DELETE_STATE, 
    "Delete saved state (current cartridge)" );
  wii_add_child( state, child );

  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( wii_menu_root, child );

  //
  // The advanced menu
  //

  TREENODE *advanced = wii_create_tree_node( NODETYPE_ADVANCED, 
    "Advanced" );
  wii_add_child( wii_menu_root, advanced );    

  child = wii_create_tree_node( NODETYPE_FILTER, 
    "Video filter" );
  wii_add_child( advanced, child );

  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( advanced, child );

  child = wii_create_tree_node( NODETYPE_DEBUG_MODE, 
    "Debug mode" );
  wii_add_child( advanced, child );

  child = wii_create_tree_node( NODETYPE_TOP_MENU_EXIT, 
    "Top menu exit" );
  wii_add_child( advanced, child );

  child = wii_create_tree_node( NODETYPE_WIIMOTE_MENU_ORIENT, 
    "Wiimote (menu)" );
  wii_add_child( advanced, child );

  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( advanced, child );

  child = wii_create_tree_node( NODETYPE_SELECT_LANG, 
    "Language" );
  wii_add_child( advanced, child );  

  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( advanced, child );

  wii_menu_push( wii_menu_root );	
}

/*
 * Updates the buffer with the header message for the current menu
 *
 * menu     The menu
 * buffer   The buffer to update with the header message for the
 *          current menu.
 */
void wii_menu_handle_get_header( TREENODE* menu, char *buffer )
{
  switch( menu->node_type )
  {
    case NODETYPE_LOAD_ROM:
      if( !games_read )
      {
        snprintf( buffer, WII_MENU_BUFF_SIZE, gettextmsg("Reading game list...") );                
      }
      break;
    case NODETYPE_LOAD_STATE:
      if( !save_states_read )
      {
        snprintf( buffer, WII_MENU_BUFF_SIZE, gettextmsg("Reading saved state list...") );                            
      }
      break;
    default:
      /* do nothing */
      break;
  }
}

/*
 * Updates the buffer with the footer message for the current menu
 *
 * menu     The menu
 * buffer   The buffer to update with the footer message for the
 *          current menu.
 */
void wii_menu_handle_get_footer( TREENODE* menu, char *buffer )
{
  switch( menu->node_type )
  {
    case NODETYPE_LOAD_ROM:
      if( games_read )
      {
        wii_get_list_footer( 
          menu, "cartridge", "cartridges", buffer );
      }
      break;
    case NODETYPE_LOAD_STATE:
       if( save_states_read )
       {
        wii_get_list_footer( 
          menu, "state save", "state saves", buffer );
       }
      break;
    default:
      break;
  }
}

/*
 * Updates the buffer with the name of the specified node
 *
 * node     The node
 * name     The name of the specified node
 * value    The value of the specified node
 */
void wii_menu_handle_get_node_name( 
  TREENODE* node, char *buffer, char* value )
{
  const char* strmode = NULL;
  int index;

  snprintf( buffer, WII_MENU_BUFF_SIZE, "%s", node->name );

  switch( node->node_type )
  {
    case NODETYPE_EMULATOR_SETTINGS:
      snprintf( 
        buffer, WII_MENU_BUFF_SIZE, "%s (%s)", 
          gettextmsg(node->name), 
          gettextmsg(emuRegistry.getCurrentEmulator()->getName()) );
      break;
    case NODETYPE_SELECT_LANG:
      snprintf( 
        value, WII_MENU_BUFF_SIZE, "%s", 
          language_menu->children[language_index]->name );
      break;
    case NODETYPE_DEBUG_MODE:
    case NODETYPE_TOP_MENU_EXIT:
    case NODETYPE_AUTO_LOAD_STATE:
    case NODETYPE_AUTO_SAVE_STATE:
    case NODETYPE_FILTER:
    case NODETYPE_VSYNC:
      {
        BOOL enabled = FALSE;
        switch( node->node_type )
        {
          case NODETYPE_VSYNC:
            enabled = ( wii_vsync == VSYNC_ENABLED );
            break;
          case NODETYPE_DEBUG_MODE:
            enabled = wii_debug;
            break;
          case NODETYPE_FILTER:
            enabled = wii_filter;
            break;
          case NODETYPE_TOP_MENU_EXIT:
            enabled = wii_top_menu_exit;
            break;
          case NODETYPE_AUTO_LOAD_STATE:
            enabled = wii_auto_load_state;
            break;
          case NODETYPE_AUTO_SAVE_STATE:
            enabled = wii_auto_save_state;
            break;
          default:
            /* do nothing */
            break;
        }
        snprintf( value, WII_MENU_BUFF_SIZE, "%s", 
          enabled ? "Enabled" : "Disabled" );
        break;
    }
    case NODETYPE_WIIMOTE_MENU_ORIENT:
      if( wii_mote_menu_vertical )
      {
        strmode="Upright";
      }
      else
      {
        strmode="Sideways";
      }
      snprintf( value, WII_MENU_BUFF_SIZE, "%s", strmode );
      break;
    default:
      {
        Emulator* emu = emuRegistry.getCurrentEmulator();
        if( emu != NULL )
        { 
          emu->getMenuManager().getNodeName( node, buffer, value );
        }
      }
      break;
  }
}

/*
 * React to the "select" event for the specified node
 *
 * node     The node
 */
void wii_menu_handle_select_node( TREENODE *node )
{
  char buff[WII_MAX_PATH];

  Emulator* emu = emuRegistry.getCurrentEmulator();
  if( emu != NULL )
  { 
    emu->getMenuManager().selectNode( node );
  }

  if( node->node_type == NODETYPE_ROM ||
      node->node_type == NODETYPE_RESUME ||
      node->node_type == NODETYPE_RESET ||
      node->node_type == NODETYPE_STATE_SAVE )
  {   
    // Essentially blanks the screen
    wii_gx_push_callback( NULL, FALSE );

    switch( node->node_type )
    {
      case NODETYPE_ROM:            
        snprintf( 
          buff, sizeof(buff), "%s%s", wii_get_roms_dir(), node->name ); 
        last_rom_index = wii_menu_get_current_index();
        wii_start_emulation( buff, NULL, false, false );
        break;
      case NODETYPE_RESUME:
        wii_resume_emulation();
        break;
      case NODETYPE_RESET:
        wii_reset_emulation();
        break;
      case NODETYPE_STATE_SAVE:
        snprintf( 
          buff, sizeof(buff), "%s%s", wii_get_saves_dir(), node->name );  
        wii_start_snapshot( buff );
        break;
      default:
        /* do nothing */
        break;
    }

    lastsavename[0] = '\0'; // Force recheck of save state
    wii_gx_pop_callback();
  }
  else
  {
    LOCK_RENDER_MUTEX();

    switch( node->node_type )
    {
      case NODETYPE_SELECT_LANG:
        language_index++;
        if( language_index >= language_menu->child_count )
        {
          language_index = 0;
        }
        select_language();
      break;
      case NODETYPE_TOP_MENU_EXIT:
        wii_top_menu_exit ^= 1;
        break;
      case NODETYPE_WIIMOTE_MENU_ORIENT:
        wii_mote_menu_vertical ^= 1;
        break;
      case NODETYPE_DEBUG_MODE:
        wii_debug ^= 1;
        break;
      case NODETYPE_FILTER:
        wii_filter ^= 1;
        break;
      case NODETYPE_AUTO_LOAD_STATE:
        wii_auto_load_state ^= 1;
        break;
      case NODETYPE_AUTO_SAVE_STATE:
        wii_auto_save_state ^= 1;
        break;
      case NODETYPE_SAVE_STATE_MANAGEMENT:
      case NODETYPE_ADVANCED:
      case NODETYPE_LOAD_ROM:               
        wii_menu_push( node );
        if( node->node_type == NODETYPE_LOAD_ROM )
        {
          wii_menu_move( node, last_rom_index );
        }
        break;
      case NODETYPE_CARTRIDGE_SETTINGS_CURRENT:
        {
          Emulator* emu = emuRegistry.getCurrentEmulator();
          if( emu != NULL )
          {
            TREENODE* menu = 
              emu->getMenuManager().getCartridgeSettingsMenu();

            if( menu != NULL )
            {
              wii_menu_push( menu );
            }
          }
        }
        break;
      case NODETYPE_EMULATOR_SETTINGS:
        {
          Emulator* emu = emuRegistry.getCurrentEmulator();
          if( emu != NULL )
          {
            TREENODE* menu = emu->getMenuManager().getEmulatorMenu();
            if( menu != NULL )
            {
              wii_menu_push( menu );
            }
          }
        }
        break;
      case NODETYPE_LOAD_STATE:
        wii_menu_clear_children( node );
        wii_menu_push( node );
        save_states_read = FALSE;
        break;
      case NODETYPE_SAVE_STATE:
        save_snapshot( NULL, TRUE );
        lastsavename[0] = '\0'; // Force recheck of save state
        break;
      case NODETYPE_DELETE_STATE:
        wii_delete_snapshot();
        lastsavename[0] = '\0'; // Force recheck of save state
        break;
      default:
        /* do nothing */
        break;
    }

    UNLOCK_RENDER_MUTEX();
  }
}

/*
 * Determines whether the node is currently visible
 *
 * node     The node
 * return   Whether the node is visible
 */
BOOL wii_menu_handle_is_node_visible( TREENODE *node )
{
  switch( node->node_type )
  {
    case NODETYPE_SAVE_STATE:
    case NODETYPE_RESET:
    case NODETYPE_RESUME:
      return wii_last_rom != NULL;
    case NODETYPE_CARTRIDGE_SETTINGS_CURRENT_SPACER:
      {
        Emulator* emu = emuRegistry.getCurrentEmulator();
        return
          emu != NULL && 
          ( ( emu->getMenuManager().getCartridgeSettingsMenu() != NULL &&
              wii_last_rom != NULL ) ||
            ( emu->getMenuManager().getEmulatorMenu() != NULL ) );
      }
    case NODETYPE_CARTRIDGE_SETTINGS_CURRENT:    
      {
        Emulator* emu = emuRegistry.getCurrentEmulator();
        return 
          emu != NULL && 
          emu->getMenuManager().getCartridgeSettingsMenu() != NULL &&
          wii_last_rom != NULL;
      }
    case NODETYPE_EMULATOR_SETTINGS:
    case NODETYPE_EMULATOR_SETTINGS_SPACER:
      {
        Emulator* emu = emuRegistry.getCurrentEmulator();
        return 
          emu != NULL && 
          emu->getMenuManager().getEmulatorMenu() != NULL;
      }
    case NODETYPE_DELETE_STATE:
      if( wii_last_rom != NULL )
      {
        char savename[WII_MAX_PATH] = "";
        wii_snapshot_handle_get_name( wii_last_rom, savename );
        if( lastsavename[0] == '\0' || strcmp( savename, lastsavename ) )
        {
          saveexists = Util_fileexists( savename );
          Util_strlcpy( lastsavename, savename, WII_MAX_PATH );
        }
        return saveexists;
      }
      return FALSE;
    default:
      {
        Emulator* emu = emuRegistry.getCurrentEmulator();
        if( emu != NULL )
        { 
          return emu->getMenuManager().isNodeVisible( node );
        }   
      }
  }

  return TRUE;
}

/*
 * Determines whether the node is selectable
 *
 * node     The node
 * return   Whether the node is selectable
 */
BOOL wii_menu_handle_is_node_selectable( TREENODE *node )
{
  if( node->node_type == NODETYPE_CARTRIDGE_SETTINGS_CURRENT_SPACER )
  {
    return FALSE;
  }

  return TRUE;
}

/*
 * Provides an opportunity for the specified menu to do something during 
 * a display cycle.
 *
 * menu     The menu
 */
void wii_menu_handle_update( TREENODE *menu )
{
  switch( menu->node_type )
  {
    case NODETYPE_LOAD_ROM:
      if( !games_read )
      {
        LOCK_RENDER_MUTEX();

        wii_read_game_list( menu );  
        wii_menu_reset_indexes();    
        wii_menu_move( menu, 1 );

        UNLOCK_RENDER_MUTEX();
      }
      break;
    case NODETYPE_LOAD_STATE:
      if( !save_states_read )
      {
        LOCK_RENDER_MUTEX();

        wii_read_save_state_list( menu );    
        wii_menu_reset_indexes();    
        wii_menu_move( menu, 1 );

        UNLOCK_RENDER_MUTEX();
      }
      break;
    default:
      /* do nothing */
      break;
  }
}

/*
 * Reads the list of languages into the specified menu
 *
 * menu     The menu to read the languages into
 */
static void read_lang_list( TREENODE *menu )
{
  DIR_ITER *langdir = diropen( wii_get_lang_dir() );
  if( langdir != NULL)
  {
    struct stat statbuf;
    char ext[WII_MAX_PATH];
    char filepath[WII_MAX_PATH];
    while( dirnext( langdir, filepath, &statbuf ) == 0 )
    {               
      if( strcmp( ".", filepath ) && strcmp( "..", filepath ) )
      {				                
        Util_getextension( filepath, ext );
        if( !strcmp( ext, WII_LANG_EXT ) && !S_ISDIR( statbuf.st_mode ) )
        {
          char dirpart[WII_MAX_PATH];
          char filepart[WII_MAX_PATH];
          Util_splitpath( filepath, dirpart, filepart );

          int idx = strlen(filepart) - strlen(WII_LANG_EXT) - 1;
          if( idx > 0 )
          {
            filepart[ idx ] = '\0';
            TREENODE *child = 
              wii_create_tree_node( NODETYPE_LANG, filepart );
            wii_add_child( menu, child );
          }
        }	
      }
    }
    dirclose( langdir );
  }
  else
  {
    wii_set_status_message( "Error opening languages directory." );
  }

  // Sort the games list
  qsort( menu->children, menu->child_count, 
    sizeof(*(menu->children)), wii_menu_name_compare );
}


/*
 * Reads the list of games into the specified menu
 *
 * menu     The menu to read the games into
 */
static void wii_read_game_list( TREENODE *menu )
{
  DIR_ITER *romdir = diropen( wii_get_roms_dir() );
  if( romdir != NULL)
  {
    struct stat statbuf;
    char filepath[WII_MAX_PATH];
    while( dirnext( romdir, filepath, &statbuf ) == 0 )
    {               
      if( ( strcmp( ".", filepath ) && strcmp( "..", filepath ) ) &&
          !S_ISDIR( statbuf.st_mode ) )
      {				                
        TREENODE *child = 
          wii_create_tree_node( NODETYPE_ROM, filepath );

        wii_add_child( menu, child );
      }
    }
    dirclose( romdir );
  }
  else
  {
    wii_set_status_message( "Error opening roms directory." );
  }

  // Sort the games list
  qsort( menu->children, menu->child_count, 
    sizeof(*(menu->children)), wii_menu_name_compare );

  games_read = 1;
}

/*
 * Reads the list of snapshots into the specified menu
 *
 * menu     The menu to read the snapshots into
 */
static void wii_read_save_state_list( TREENODE *menu )
{
  DIR_ITER *ssdir = diropen( wii_get_saves_dir() );
  if( ssdir != NULL)
  {   
    struct stat statbuf;
    char ext[WII_MAX_PATH];
    char filepath[WII_MAX_PATH];
    while( dirnext( ssdir, filepath, &statbuf ) == 0 ) 
    {            
      if( strcmp( ".", filepath ) && strcmp( "..", filepath ) )
      {			                
        Util_getextension( filepath, ext );
        if( !strcmp( ext, WII_SAVE_GAME_EXT ) && 
            !S_ISDIR( statbuf.st_mode ) )
        {
          // TODO: Check to see if a rom exists for the snapshot
          // TODO: Provide option to display cart info from 
          //       header
          TREENODE *child = 
            wii_create_tree_node( NODETYPE_STATE_SAVE, filepath );

          wii_add_child( menu, child );
        }				
      }
    }

    dirclose( ssdir );
  }
  else
  {
    wii_set_status_message( "Error opening state saves directory." );
  }

  // Sort the games list
  qsort( menu->children, menu->child_count, 
    sizeof(*(menu->children)), wii_menu_name_compare );

  save_states_read = TRUE;
}

/*
 * Invoked after exiting the menu loop
 */
void wii_menu_handle_post_loop()
{
}

/*
 * Invoked prior to entering the menu loop
 */
void wii_menu_handle_pre_loop()
{
}

/*
 * Invoked when the home button is pressed when the 
 * menu is being displayed
 */
void wii_menu_handle_home_button()
{
}