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

#ifndef WII_MEDNAFEN_H
#define WII_MEDNAFEN_H

#include <wiiuse/wpad.h>
#include "wii_main.h"

// Wii information
#define WII_WIDTH 640
#define WII_HEIGHT 480

#define WII_WIDTH_DIV2 320
#define WII_HEIGHT_DIV2 240

// Virtual boy size
#define VB_WIDTH 384
#define VB_HEIGHT 224

/** The last cartridge hash */
extern char wii_cartridge_hash[33];
/** The cartridge hash with header (may be the same) */
extern char wii_cartridge_hash_with_header[33];
/** Whether to display debug info (FPS, etc.) */
extern BOOL wii_debug;
/** Whether to filter the display */
extern BOOL wii_filter;
/** Whether cheat mode is enabled */
extern BOOL wii_cheats;
/** Hardware buttons (reset, power, etc.) */
extern u8 wii_hw_button;
/** The current language */
extern char wii_language[WII_MAX_PATH];
/** Whether to auto load/save state */
extern BOOL wii_auto_load_save_state;
/** Whether to use the GX/VI scaler */
extern BOOL wii_gx_vi_scaler;
/** Volume */
extern int wii_volume;
/** Whether to enable rewind */
extern BOOL wii_rewind;
/** Whether to automatically add rewind buttons */
extern BOOL wii_rewind_add_buttons;

/**
 * Returns the roms directory
 *
 * @return  The roms directory
 */
char* wii_get_roms_dir();

/**
 * Sets the current rom directory
 *
 * @param   newDir The new roms directory
 */
void wii_set_roms_dir(const char* newDir);

/**
 * Returns the lang directory
 *
 * @param   return  The lang directory
 */
char* wii_get_lang_dir();

/**
 * Returns the base directory
 *
 * @return  The base directory
 */
char* wii_get_base_dir();

#endif
