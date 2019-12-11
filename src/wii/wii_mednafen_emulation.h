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

#ifndef WII_MEDNAFEN_EMULATION_H
#define WII_MEDNAFEN_EMULATION_H

/**
 * Starts the emulator for the specified rom file.
 *
 * @param   romfile The rom file to run in the emulator
 * @param   savefile The name of the save file to load. If this value is NULL,
 *          no save is explicitly loaded (auto-load may occur). If the value is
 *          "", no save is loaded and auto-load is ignored (used for reset).
 * @param   reset  Whether to reset the current game
 * @param   resume Whether to resume the current game
 */
void wii_start_emulation(char* romfile,
                         const char* savefile = NULL,
                         bool reset = false,
                         bool resume = false);

/**
 * Resumes emulation of the current game
 */
void wii_resume_emulation();

/**
 * Resets the current game
 */
void wii_reset_emulation();

#endif
