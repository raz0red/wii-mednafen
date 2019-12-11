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

#ifndef WII_MEDNAFEN_MAIN_H
#define WII_MEDNAFEN_MAIN_H

#include "Emulator.h"

/**
 * Initializes the emulator
 */
void wii_mednafen_init();

/**
 * Free resources (closes) the emulator
 */
void wii_mednafen_free();

/**
 * Loads the specified game
 *
 * @param   game The game to load
 * @return  1 if the load is successful, 0 if it fails
 */
int wii_mednafen_load_game(char* game);

/**
 * The emulation loop
 *
 * @param   resume Whether we are resuming
 */
void wii_mednafen_emu_loop(BOOL resume);

/**
 * Displays a screen indicating the controls configuration for the current
 * cartridge
 *
 * @return  Returns 1 if we should continue past the screen
 */
int wii_mednafen_show_controls_screen();

/**
 * Displays a message during emulation
 *
 * @param   message The message to display
 */
void wii_mednafen_set_message(const char* message);

/**
 * Returns the last display rect
 *
 * @return  The last display rect
 */
Rect* wii_mednafen_get_last_rect();

/**
 * Resets the last cached display rect size
 */
void wii_mednafen_reset_last_rect();

/**
 * Whether the controls should be reset prior to entering the emulator loop.
 */
void wii_mednafen_set_reset_controls();

#endif
