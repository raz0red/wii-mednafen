/*
WiiMednafen : Wii port of the Mednafen emulator

Copyright (C) 2011
raz0red

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

#include "main.h"

#include <stdio.h>
#include <stdlib.h>

#include "wii_app.h"
#include "wii_mednafen_resize_screen.h"
#include "wii_sdl.h"
#include "wii_snapshot.h"
#include "wii_util.h"
#include "wii_gx.h"
#include "fileop.h"
#include "networkop.h"

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

// Mednafen external references
extern MDFNGI *MDFNGameInfo;

// Whether we are loading a game
BOOL loading_game = FALSE;
// Have we read the games list yet?
static BOOL games_read = FALSE;
// Whether we are pending a drive mount
static BOOL mount_pending = TRUE;
// The index of the last rom that was run
static s16 last_rom_index = 1;
// The language menu
static TREENODE *language_menu;
// The roms node
static TREENODE *roms_menu;
// The current language index
static u8 language_index = 0;

// Forward refs
static void wii_read_game_list( TREENODE *menu );
static void read_lang_list( TREENODE *menu );

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
  roms_menu = child;
  wii_add_child( wii_menu_root, child );

  //
  // Save state management
  //

  child = wii_create_tree_node( NODETYPE_CARTRIDGE_SETTINGS_CURRENT_SPACER, "" );
  wii_add_child( wii_menu_root, child );
  
  TREENODE* states = wii_create_tree_node( 
    NODETYPE_CARTRIDGE_SAVE_STATES, "Save states" );
  wii_add_child( wii_menu_root, states );

  child = wii_create_tree_node( 
    NODETYPE_CARTRIDGE_SAVE_STATES_SLOT, "Slot" );
  wii_add_child( states, child );

  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( states, child );

  child = wii_create_tree_node( NODETYPE_SAVE_STATE, "Save state" );
  wii_add_child( states, child );

  child = wii_create_tree_node( NODETYPE_LOAD_STATE, "Load state" );
  wii_add_child( states, child );

  TREENODE *cart_settings = wii_create_tree_node( 
    NODETYPE_CARTRIDGE_SETTINGS_CURRENT, "Game-specific settings" );
  wii_add_child( wii_menu_root, cart_settings );    

  child = wii_create_tree_node( NODETYPE_EMULATOR_SETTINGS_SPACER, "" );
  wii_add_child( wii_menu_root, child );

  TREENODE *emulator_settings = wii_create_tree_node( 
    NODETYPE_EMULATOR_SETTINGS, "Emulator settings" );                                                        
  wii_add_child( wii_menu_root, emulator_settings );

  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( wii_menu_root, child );

  //
  // The advanced menu
  //

  TREENODE *advanced = wii_create_tree_node( NODETYPE_ADVANCED, 
    "Advanced" );
  wii_add_child( wii_menu_root, advanced );    

  TREENODE *video_settings = wii_create_tree_node( 
    NODETYPE_VIDEO_SETTINGS, "Video settings" );                                                        
  wii_add_child( advanced, video_settings );

  child = wii_create_tree_node( NODETYPE_FULL_WIDESCREEN, 
    "Full widescreen" );
  wii_add_child( video_settings, child );
  child = wii_create_tree_node( NODETYPE_16_9_CORRECTION, 
    "16:9 correction" );
  wii_add_child( video_settings, child );  

#if 0
  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( video_settings, child );
#endif

  child = wii_create_tree_node( NODETYPE_DOUBLE_STRIKE, 
    "Double strike (240p)" );
  wii_add_child( video_settings, child );

#if 0
  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( video_settings, child );
#endif

  child = wii_create_tree_node( NODETYPE_FILTER, 
    "Bilinear filter" );
  wii_add_child( video_settings, child );

  child = wii_create_tree_node( NODETYPE_TRAP_FILTER, 
    "Color trap filter" );
  wii_add_child( video_settings, child );

#if 0
  child = wii_create_tree_node( NODETYPE_GX_VI_SCALER_SPACER, "" );
  wii_add_child( video_settings, child );
#endif

  child = wii_create_tree_node( NODETYPE_GX_VI_SCALER, "Scaler" );
  wii_add_child( video_settings, child );

  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( advanced, child );

  child = wii_create_tree_node( NODETYPE_VOLUME, "Volume" );
  wii_add_child( advanced, child );

  child = wii_create_tree_node( NODETYPE_REWIND, "Rewind" );
  wii_add_child( advanced, child );

  child = wii_create_tree_node( NODETYPE_REWIND_BUTTON, "Rewind buttons" );
  wii_add_child( advanced, child );

#if 0
  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( advanced, child );
#endif

  child = wii_create_tree_node( NODETYPE_CHEATS, "Cheats" );
  wii_add_child( advanced, child );

  child = wii_create_tree_node( NODETYPE_SELECT_LANG, "Language" );
  wii_add_child( advanced, child );  

#if 0
  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( advanced, child );
#endif

  child = wii_create_tree_node( NODETYPE_TOP_MENU_EXIT, 
    "Top menu exit" );
  wii_add_child( advanced, child );

  child = wii_create_tree_node( NODETYPE_WIIMOTE_MENU_ORIENT, 
    "Wiimote (menu)" );
  wii_add_child( advanced, child );

  child = wii_create_tree_node( NODETYPE_SPACER, "" );
  wii_add_child( advanced, child );

  child = wii_create_tree_node( NODETYPE_DEBUG_MODE, 
    "Debug mode" );
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
  if( loading_game )
  {
    snprintf( buffer, WII_MENU_BUFF_SIZE, gettextmsg("Loading game...") );
  }
  else
  {
    switch( menu->node_type )
    {
      case NODETYPE_LOAD_ROM:    
        if( !games_read )
        {
          snprintf( buffer, WII_MENU_BUFF_SIZE, 
            mount_pending ? 
              gettextmsg("Attempting to mount drive...") :
              gettextmsg("Reading file list...") );
        }
        break;
      default:
        /* do nothing */
        break;
    }
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
  if( loading_game )
  {
    snprintf( buffer, WII_MENU_BUFF_SIZE, " " );
  }
  else
  {
    switch( menu->node_type )
    {
      case NODETYPE_LOAD_ROM:
        if( games_read )
        {
          wii_get_list_footer( 
            roms_menu, "item", "items", buffer );
        }
        break;
      default:
        break;
    }
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
    case NODETYPE_ROOT_DRIVE:
      {
        int device;
        FindDevice( node->name, &device );
        switch( device )
        {
          case DEVICE_SD:
            snprintf( buffer, WII_MENU_BUFF_SIZE, "[%s]", 
              gettextmsg( "SD Card" ) );
            break;
          case DEVICE_USB:
            snprintf( buffer, WII_MENU_BUFF_SIZE, "[%s]",
              gettextmsg( "USB Device" ) );
            break;
          case DEVICE_SMB:
            snprintf( buffer, WII_MENU_BUFF_SIZE, "[%s]",
              gettextmsg( "Network Share" ) );
            break;
        }
      }
      break;
    case NODETYPE_DIR:
      snprintf( buffer, WII_MENU_BUFF_SIZE, "[%s]", node->name );
      break;
    case NODETYPE_CARTRIDGE_SAVE_STATES_SLOT:
      {
        BOOL isLatest;
        int current = wii_snapshot_current( &isLatest );
        current++;
        if( !isLatest )
        {
          snprintf( value, WII_MENU_BUFF_SIZE, "%d", current );
        }
        else
        {
          snprintf( value, WII_MENU_BUFF_SIZE, "%d (%s)", 
            current, gettextmsg( "Latest" ) );
        }
      }
      break;
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
    case NODETYPE_FULL_WIDESCREEN:
      snprintf( value, WII_MENU_BUFF_SIZE, "%s", 
        ( wii_full_widescreen == WS_AUTO ? "(auto)" :
          ( wii_full_widescreen ? "Enabled" : "Disabled" ) ) );
      break;
    case NODETYPE_GX_VI_SCALER:
      snprintf( value, WII_MENU_BUFF_SIZE, "%s", 
        ( wii_gx_vi_scaler ? "GX + VI" : "GX" ) );
      break;
    case NODETYPE_VOLUME:
      snprintf( value, WII_MENU_BUFF_SIZE, "%d %s", 
        ( wii_volume / 10 ), 
        ( wii_volume == 100 ?  gettextmsg("(normal)") : "" ) );
      break;
    case NODETYPE_REWIND_BUTTON:
      snprintf( value, WII_MENU_BUFF_SIZE, "%s", 
        wii_rewind_add_buttons ? "Auto" : "Manual" );
      break;
    case NODETYPE_DEBUG_MODE:
    case NODETYPE_TOP_MENU_EXIT:
    case NODETYPE_FILTER:
    case NODETYPE_VSYNC:
    case NODETYPE_AUTO_LOAD_SAVE:
    case NODETYPE_DOUBLE_STRIKE:
    case NODETYPE_CHEATS:    
    case NODETYPE_TRAP_FILTER:
    case NODETYPE_16_9_CORRECTION:
    case NODETYPE_REWIND:
      {
        BOOL enabled = FALSE;
        switch( node->node_type )
        {
          case NODETYPE_REWIND:
            enabled = wii_rewind;
            break;
          case NODETYPE_16_9_CORRECTION:
            enabled = wii_16_9_correction;
            break;
          case NODETYPE_TRAP_FILTER:
            enabled = wii_trap_filter;
            break;
          case NODETYPE_DOUBLE_STRIKE:
            enabled = wii_double_strike_mode;
            break;
          case NODETYPE_CHEATS:
            enabled = wii_cheats;
            break;
          case NODETYPE_AUTO_LOAD_SAVE:
            enabled = wii_auto_load_save_state;
            break;
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
      node->node_type == NODETYPE_LOAD_STATE ||
      node->node_type == NODETYPE_RESET )
  {   
    // Essentially blanks the screen
    //wii_gx_push_callback( NULL, FALSE, NULL );

    switch( node->node_type )
    {
      case NODETYPE_LOAD_STATE:
        wii_start_snapshot();
        break;
      case NODETYPE_ROM:            
        snprintf( 
          buff, sizeof(buff), "%s%s", wii_get_roms_dir(), node->name ); 
        last_rom_index = wii_menu_get_current_index();
        loading_game = TRUE;
        wii_start_emulation( buff, "", false, false );
        loading_game = FALSE;
        break;
      case NODETYPE_RESUME:
        wii_resume_emulation();
        break;
      case NODETYPE_RESET:
        wii_reset_emulation();
        break;
      default:
        /* do nothing */
        break;
    }

    //wii_gx_pop_callback();
  }
  else
  {
    LOCK_RENDER_MUTEX();

    switch( node->node_type )
    {
      case NODETYPE_VOLUME:
        wii_volume += 10;
        if( wii_volume > 150 )
        {
          wii_volume = 0;
        }
        break;
      case NODETYPE_SAVE_STATE:
        wii_save_snapshot( NULL, TRUE );
        break;
      case NODETYPE_CARTRIDGE_SAVE_STATES_SLOT:
        wii_snapshot_next();
        break;
      case NODETYPE_SELECT_LANG:
        language_index++;
        if( language_index >= language_menu->child_count )
        {
          language_index = 0;
        }
        select_language();
        break;
      case NODETYPE_DOUBLE_STRIKE:
        wii_double_strike_mode ^= 1;
        break;
      case NODETYPE_FULL_WIDESCREEN:
        wii_full_widescreen++;
        if( wii_full_widescreen > WS_AUTO )
        {
          wii_full_widescreen = 0;
        }
        break;
      case NODETYPE_16_9_CORRECTION:
        wii_16_9_correction ^= 1;
        break;
      case NODETYPE_GX_VI_SCALER:
        wii_gx_vi_scaler ^= 1;
        break;
      case NODETYPE_AUTO_LOAD_SAVE:
        wii_auto_load_save_state ^= 1;
        break;
      case NODETYPE_TOP_MENU_EXIT:
        wii_top_menu_exit ^= 1;
        break;
      case NODETYPE_CHEATS:
        wii_cheats ^= 1;
        break;
      case NODETYPE_REWIND:
        wii_rewind ^= 1;
        if( wii_rewind )
        {
          wii_set_status_message( 
            "Note: Enabling rewind may affect performance." );
        }
        break;
      case NODETYPE_REWIND_BUTTON:
        wii_rewind_add_buttons ^= 1;
        break;
      case NODETYPE_WIIMOTE_MENU_ORIENT:
        wii_mote_menu_vertical ^= 1;
        break;
      case NODETYPE_DEBUG_MODE:
        wii_debug ^= 1;
        break;
      case NODETYPE_TRAP_FILTER:
        wii_trap_filter ^= 1;
        break;
      case NODETYPE_FILTER:
        wii_filter ^= 1;
        break;
      case NODETYPE_ROOT_DRIVE:
      case NODETYPE_UPDIR:
      case NODETYPE_DIR:
        if( node->node_type == NODETYPE_ROOT_DRIVE )
        {
          char path[WII_MAX_PATH];
          snprintf( path, sizeof(path), "%s/", node->name );
          wii_set_roms_dir( path );
          mount_pending = TRUE;
        }
        else if( node->node_type == NODETYPE_UPDIR )
        {
          const char* romsDir = wii_get_roms_dir();
          int len = strlen( romsDir );
          if( len > 1 && romsDir[len-1] == '/' )
          {
            char dirpart[WII_MAX_PATH] = "";
            char filepart[WII_MAX_PATH] = "";
            Util_splitpath( romsDir, dirpart, filepart );
            len = strlen(dirpart);
            if( len > 0 )
            {
              dirpart[len] = '/';
              dirpart[len+1] = '\0';
            }
            wii_set_roms_dir( dirpart );
          }
        }
        else
        {
          char newDir[WII_MAX_PATH];
          snprintf( newDir, sizeof(newDir), "%s%s/", 
            wii_get_roms_dir(), node->name );
          wii_set_roms_dir( newDir );
        }
        games_read = FALSE;
        last_rom_index = 1;
        break;
      case NODETYPE_ADVANCED:
      case NODETYPE_VIDEO_SETTINGS:
      case NODETYPE_CARTRIDGE_SAVE_STATES:
      case NODETYPE_LOAD_ROM:               
        wii_menu_push( node );
        if( node->node_type == NODETYPE_LOAD_ROM )
        {
          if( games_read )
          {
            wii_menu_move( node, last_rom_index );  
          }
        }
        else if( node->node_type == NODETYPE_CARTRIDGE_SAVE_STATES )
        {
          // Initialize the "current" value prior to displaying
          // the menu...
          BOOL foo;
          wii_snapshot_current( &foo );
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
    case NODETYPE_LOAD_STATE:
      return wii_snapshot_current_exists();
    case NODETYPE_REWIND_BUTTON:
      return wii_rewind;
    case NODETYPE_GX_VI_SCALER_SPACER:
    case NODETYPE_GX_VI_SCALER:
      return !wii_filter;
    case NODETYPE_RESET:
    case NODETYPE_RESUME:
    case NODETYPE_EMULATOR_SETTINGS_SPACER:
      return wii_last_rom != NULL;
    case NODETYPE_CARTRIDGE_SAVE_STATES:
      return wii_last_rom != NULL && MDFNGameInfo->StateAction != NULL;
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
      {
        Emulator* emu = emuRegistry.getCurrentEmulator();
        return 
          emu != NULL && 
          emu->getMenuManager().getEmulatorMenu() != NULL;
      }
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
  if( node->node_type == NODETYPE_CARTRIDGE_SETTINGS_CURRENT_SPACER ||      
      node->node_type == NODETYPE_EMULATOR_SETTINGS_SPACER ||
      node->node_type == NODETYPE_GX_VI_SCALER_SPACER )
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

        if( mount_pending )
        {
          const char* roms = wii_get_roms_dir();
          if( strlen( roms ) > 0 )
          {
            char mount[WII_MAX_PATH];
            Util_strlcpy( mount, roms, sizeof(mount) );

            resetSmbErrorMessage(); // Reset the SMB error message
            if( !ChangeInterface( mount, FS_RETRY_COUNT ) )
            {
              wii_set_roms_dir( "" );
              const char* netMsg = getSmbErrorMessage();
              if( netMsg != NULL )
              {
                wii_set_status_message( netMsg );
              }
              else
              {
                char msg[256];
                snprintf( msg, sizeof(msg), "%s: %s", 
                  gettextmsg("Unable to mount"), mount );
                wii_set_status_message( msg );
              }
            }
          }
          mount_pending = FALSE;
        }

        wii_read_game_list( roms_menu );  
        wii_menu_reset_indexes();    
        wii_menu_move( roms_menu, 1 );

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
  DIR *langdir = opendir( wii_get_lang_dir() );
  if( langdir != NULL)
  {
    struct dirent* entry = NULL;
    char ext[WII_MAX_PATH];
    while( ( entry = readdir( langdir ) ) != NULL )
    {               
      if( ( strcmp( ".", entry->d_name ) && strcmp( "..", entry->d_name ) ) &&
          ( entry->d_type != DT_DIR ) )
      {				                
        Util_getextension( entry->d_name, ext );
        if( !strcmp( ext, WII_LANG_EXT ) )
        {
          char filepart[WII_MAX_PATH];
          Util_strlcpy( filepart, entry->d_name, sizeof(filepart) );
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
    closedir( langdir );
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
* Used for comparing menu names when sorting (qsort)
*
* a        The first tree node to compare
* b        The second tree node to compare
* return   The result of the comparison
*/
static int game_name_compare( const void *a, const void *b )
{
  TREENODE** aptr = (TREENODE**)a;
  TREENODE** bptr = (TREENODE**)b;
  int type = (*aptr)->node_type - (*bptr)->node_type;
  return type != 0 ? type : stricmp( (*aptr)->name, (*bptr)->name );
}

/*
 * Reads the list of games into the specified menu
 *
 * menu     The menu to read the games into
 */
static void wii_read_game_list( TREENODE *menu )
{
  const char* roms = wii_get_roms_dir();

  wii_menu_clear_children( menu ); // Clear the children

#ifdef WII_NETTRACE
net_print_string( NULL, 0, "ReadGameList: %s\n", roms, strlen(roms) );
#endif

  BOOL success = FALSE;
  if( strlen( roms ) > 0 )
  {
    DIR *romdir = opendir( roms );

#ifdef WII_NETTRACE
net_print_string( NULL, 0, "OpenDir: %d\n", roms, romdir);
#endif

    if( romdir != NULL)
    {
      wii_add_child(
        menu, wii_create_tree_node( NODETYPE_UPDIR, "[..]" ) );

      struct dirent* entry = NULL;
      while( ( entry = readdir( romdir ) ) != NULL )
      {               
        if( ( strcmp( ".", entry->d_name ) && strcmp( "..", entry->d_name ) ) )
        {				                
          TREENODE *child = 
            wii_create_tree_node( 
              ( entry->d_type == DT_DIR ? NODETYPE_DIR : NODETYPE_ROM ), 
              entry->d_name );

          wii_add_child( menu, child );
        }
      }
      closedir( romdir );

      // Sort the games list
      qsort( menu->children, menu->child_count, 
        sizeof(*(menu->children)), game_name_compare );

      success = TRUE;
    }
    else
    {
      char msg[256];
      snprintf( msg, sizeof(msg), "%s: %s", 
        gettextmsg("Error opening"), roms );
      wii_set_status_message( msg );
    }
  }

  if( !success )
  {
    wii_set_roms_dir( "" );
    wii_add_child( menu, 
      wii_create_tree_node( NODETYPE_ROOT_DRIVE, "sd:" ) );
    wii_add_child( menu, 
      wii_create_tree_node( NODETYPE_ROOT_DRIVE, "usb:" ) );
    wii_add_child( menu, 
      wii_create_tree_node( NODETYPE_ROOT_DRIVE, "smb:" ) );
  }
  
  games_read = TRUE;
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