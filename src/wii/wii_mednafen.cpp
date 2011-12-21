/*
WiiMednafen : Wii port of the Mednafen emulator

Copyright (C) 2011 raz0red

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

#include "wii_app.h"
#include "wii_config.h"
#include "wii_input.h"
#include "wii_sdl.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"
#include "wii_mednafen_menu.h"

#include "FreeTypeGX.h"

#include "font_ttf.h"
#if 0
#include "asot_ttf.h"
#endif

#include "Emulators.h"

#ifdef MEM2
#include "mem2.h"
#endif

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"  
#endif

extern "C" {
  void WII_VideoStop();
  void WII_SetDebugCallback( void (*cb)(char* msg) );
}

// The last cartridge hash
char wii_cartridge_hash[33];
// The cartridge hash with header (may be the same)
char wii_cartridge_hash_with_header[33];
// Whether to display debug info (FPS, etc.)
BOOL wii_debug = FALSE;
// Whether to filter the display
BOOL wii_filter = FALSE; 
// Whether cheat mode is enabled
BOOL wii_cheats = FALSE;
// Whether to auto load/save state
BOOL wii_auto_load_save_state = FALSE;
// The current language
char wii_language[WII_MAX_PATH] = "";
// Whether to use the GX/VI scaler
BOOL wii_gx_vi_scaler = TRUE;
// Whether to enable rewind
BOOL wii_rewind = FALSE;
// Whether to automatically add rewind buttons
BOOL wii_rewind_add_buttons = TRUE;
// Volume
int wii_volume = 100;
// The roms dir
static char roms_dir[WII_MAX_PATH] = "";

#ifdef WII_NETTRACE
static void sdlTrace( char* msg )
{
  net_print_string( NULL, 0, msg );
}
#endif

/*
 * Initializes the application
 */
void wii_handle_init()
{  
#ifdef MEM2
  InitMem2Manager();
#endif

#if 0
  // Set the default roms dir
  snprintf( 
    roms_dir, WII_MAX_PATH, "%s%s", wii_get_fs_prefix(), WII_ROMS_DIR );
#endif

#ifdef WII_NETTRACE
//  WII_SetDebugCallback( sdlTrace );
#endif
 
  // Read the config values
  wii_read_config();

  // Startup the SDL
  if( !wii_sdl_init() ) 
  {
    fprintf( stderr, "FAILED : Unable to init SDL: %s\n", SDL_GetError() );
    exit( EXIT_FAILURE );
  }

  // FreeTypeGX
  InitFreeType( (uint8_t*)font_ttf, (FT_Long)font_ttf_size  );

  // Initializes the menu
  wii_mednafen_menu_init();

  // Initializes emulator
  wii_mednafen_init();
}

/*
 * Frees resources prior to the application exiting
 */
void wii_handle_free_resources()
{  
  // We be done, write the config settings, free resources and exit
  wii_write_config();

  // Free resources from the emulator
  wii_mednafen_free();

  wii_sdl_free_resources();
  SDL_Quit();
}

/*
 * Runs the application (main loop)
 */
void wii_handle_run()
{
  //WII_VideoStop();

  // Show the menu
  wii_menu_show();
}

/*
 * Returns the roms directory
 *
 * return   The roms directory
 */
char* wii_get_roms_dir()
{
  return roms_dir;
}

/*
 * Sets the current rom directory
 *
 * newDir   The new roms directory
 */
void wii_set_roms_dir( const char* newDir )
{
  Util_strlcpy( roms_dir, newDir, sizeof(roms_dir) );
#ifdef WII_NETTRACE
  net_print_string( NULL, 0, "RomsDir: \"%s\"\n", roms_dir );
#endif
}

// The lang dir
static char lang_dir[WII_MAX_PATH] = "";

/*
 * Returns the lang directory
 *
 * return   The roms directory
 */
char* wii_get_lang_dir()
{
  if( lang_dir[0] == '\0' )
  {
    snprintf( 
      lang_dir, WII_MAX_PATH, "%s%s", wii_get_fs_prefix(), WII_LANG_DIR );
  }

  return lang_dir;
}

// The base dir
static char base_dir[WII_MAX_PATH] = "";

/*
 * Returns the base directory
 *
 * return   The bae directory
 */
char* wii_get_base_dir()
{
  if( base_dir[0] == '\0' )
  {
    snprintf( 
      base_dir, WII_MAX_PATH, "%s%s", wii_get_fs_prefix(), WII_FILES_DIR );
    base_dir[strlen(base_dir)-1]='\0'; // Remove the trailing slash
  }

  return base_dir;
}