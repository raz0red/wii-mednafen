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

#include "wii_app.h"
#include "wii_config.h"
#include "wii_input.h"
#include "wii_sdl.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"
#include "wii_mednafen_menu.h"

#include "FreeTypeGX.h"

#include "font_ttf.h"
#include "asot_ttf.h"

#include "Emulators.h"

extern "C" {
  void WII_VideoStop();
}

// The last cartridge hash
char wii_cartridge_hash[33];
// The cartridge hash with header (may be the same)
char wii_cartridge_hash_with_header[33];
// Whether to display debug info (FPS, etc.)
BOOL wii_debug = FALSE;
// Whether to filter the display
BOOL wii_filter = FALSE;
// Auto save state?
BOOL wii_auto_save_state = FALSE;
// Auto load state?
BOOL wii_auto_load_state = TRUE;
// The current language
char wii_language[WII_MAX_PATH] = "";

/*
 * Initializes the application
 */
void wii_handle_init()
{  
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

// The roms dir
static char roms_dir[WII_MAX_PATH] = "";

/*
 * Returns the roms directory
 *
 * return   The roms directory
 */
char* wii_get_roms_dir()
{
  if( roms_dir[0] == '\0' )
  {
    snprintf( 
      roms_dir, WII_MAX_PATH, "%s%s", wii_get_fs_prefix(), WII_ROMS_DIR );
  }

  return roms_dir;
}

// The saves dir
static char saves_dir[WII_MAX_PATH] = "";

/*
 * Returns the saves directory
 *
 * return   The saves directory
 */
char* wii_get_saves_dir()
{
  if( saves_dir[0] == '\0' )
  {
    snprintf( 
      saves_dir, WII_MAX_PATH, "%s%s", wii_get_fs_prefix(), WII_SAVES_DIR );
  }

  return saves_dir;
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