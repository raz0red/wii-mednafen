/*--------------------------------------------------------------------------*\
|  __      __.__.__   _____             .___             _____               |
| /  \    /  \__|__| /     \   ____   __| _/____ _____ _/ ____\____   ____   |
| \   \/\/   /  |  |/  \ /  \_/ __ \ / __ |/    \\__  \\   __\/ __ \ /    \  |
|  \        /|  |  /    Y    \  ___// /_/ |   |  \/ __ \|  | \  ___/|   |  \ |
|   \__/\  / |__|__\____|__  /\___  >____ |___|  (____  /__|  \___  >___|  / |
|        \/                \/     \/     \/    \/     \/          \/     \/  |
|                                                                            |
|    WiiMednafen by raz0red                                                  |
|    Wii port of the Mednafen emulator                                       |
|                                                                            |
|    [github.com/raz0red/wii-mednafen]                                       |
|                                                                            |
+----------------------------------------------------------------------------+
|                                                                            |
|    This program is free software; you can redistribute it and/or           |
|    modify it under the terms of the GNU General Public License             |
|    as published by the Free Software Foundation; either version 2          |
|    of the License, or (at your option) any later version.                  |
|                                                                            |
|    This program is distributed in the hope that it will be useful,         |
|    but WITHOUT ANY WARRANTY; without even the implied warranty of          |
|    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           |
|    GNU General Public License for more details.                            |
|                                                                            |
|    You should have received a copy of the GNU General Public License       |
|    along with this program; if not, write to the Free Software             |
|    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA           |
|    02110-1301, USA.                                                        |
|                                                                            |
\*--------------------------------------------------------------------------*/

#include "wii_app_common.h"
#include "wii_app.h"
#include "wii_config.h"
#include "wii_input.h"
#include "wii_sdl.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"
#include "wii_mednafen_menu.h"
#include "wii_mednafen_emulation.h"

#include "FreeTypeGX.h"
#include "font_ttf.h"

#include "Emulators.h"

#ifdef MEM2
#include "mem2.h"
#endif

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"  
#endif


/** SDL Video external references */
extern "C" {
void WII_VideoStop();
void WII_SetDebugCallback(void (*cb)(char* msg));
}

/** The last cartridge hash */
char wii_cartridge_hash[33];
/** The cartridge hash with header (may be the same) */
char wii_cartridge_hash_with_header[33];
/** Whether to display debug info (FPS, etc.) */
BOOL wii_debug = FALSE;
/** Whether to filter the display */
BOOL wii_filter = FALSE; 
/** Whether cheat mode is enabled */
BOOL wii_cheats = FALSE;
/** Whether to auto load/save state */
BOOL wii_auto_load_save_state = FALSE;
/** The current language */
char wii_language[WII_MAX_PATH] = "";
/** Whether to use the GX/VI scaler */
BOOL wii_gx_vi_scaler = TRUE;
/** Whether to enable rewind */
BOOL wii_rewind = FALSE;
/** Whether to automatically add rewind buttons */
BOOL wii_rewind_add_buttons = TRUE;
/** Volume */
int wii_volume = 100;
/** The roms dir */
static char roms_dir[WII_MAX_PATH] = "";

#ifdef WII_NETTRACE
static void sdlTrace(char* msg) {
    net_print_string(NULL, 0, msg);
}
#endif

/**
 * Returns the base directory for the application
 *
 * @return  The base directory for the application
 */
const char* wii_get_app_base_dir() {
    return WII_BASE_APP_DIR;
}

/**
 * Returns the location of the config file
 *
 * @return  The location of the config file
 */
const char* wii_get_config_file_path() {
    return WII_CONFIG_FILE;
}

/**
 * Returns the location of the data directory
 *
 * @return  The location of the data directory
 */
const char* wii_get_data_path() {
    return WII_FILES_DIR;
}

/**
 * Initializes the application
 */
void wii_handle_init() {
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
    if (!wii_sdl_init()) {
        fprintf(stderr, "FAILED : Unable to init SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // FreeTypeGX
    InitFreeType((uint8_t*)font_ttf, (FT_Long)font_ttf_size);

    // Initializes the menu
    wii_mednafen_menu_init();

    // Initializes emulator
    wii_mednafen_init();
}

/**
 * Frees resources prior to the application exiting
 */
void wii_handle_free_resources() {
    // We be done, write the config settings, free resources and exit
    wii_write_config();

    // Free resources from the emulator
    wii_mednafen_free();

    wii_sdl_free_resources();
    SDL_Quit();
}

/**
 * Runs the application (main loop)
 */
void wii_handle_run() {
    // WII_VideoStop();

    // Start specified rom (if applicable)
    if (wii_initial_rom[0] != '\0') {
        wii_start_emulation(wii_initial_rom);
    }

    // Show the menu
    wii_menu_show();
}

/**
 * Returns the roms directory
 *
 * return   The roms directory
 */
char* wii_get_roms_dir() {
    return roms_dir;
}

/**
 * Sets the current rom directory
 *
 * @param   newDir The new roms directory
 */
void wii_set_roms_dir(const char* newDir) {
    Util_strlcpy(roms_dir, newDir, sizeof(roms_dir));
#ifdef WII_NETTRACE
    net_print_string(NULL, 0, "RomsDir: \"%s\"\n", roms_dir);
#endif
}

/** The lang dir */
static char lang_dir[WII_MAX_PATH] = "";

/**
 * Returns the lang directory
 *
 * @return  The lang directory
 */
char* wii_get_lang_dir() {
    if (lang_dir[0] == '\0') {
        snprintf(lang_dir, WII_MAX_PATH, "%s%s", wii_get_fs_prefix(),
                 WII_LANG_DIR);
    }

    return lang_dir;
}

/** The base dir */
static char base_dir[WII_MAX_PATH] = "";

/**
 * Returns the base directory
 *
 * @return  The base directory
 */
char* wii_get_base_dir() {
    if (base_dir[0] == '\0') {
        snprintf(base_dir, WII_MAX_PATH, "%s%s", wii_get_fs_prefix(),
                 WII_FILES_DIR);
        base_dir[strlen(base_dir) - 1] = '\0';  // Remove the trailing slash
    }

    return base_dir;
}