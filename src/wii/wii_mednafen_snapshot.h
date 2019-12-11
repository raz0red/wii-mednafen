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

#ifndef WII_MEDNAFEN_SNAPSHOT_H
#define WII_MEDNAFEN_SNAPSHOT_H

#include <gccore.h>

/**
 * Starts the emulator for the specified snapshot file.
 *
 * @param   savefile The name of the save file to load. 
 */
BOOL wii_start_snapshot();

/**
 * Resets snapshot related information. This method is typically invoked when
 * a new rom file is loaded.
 */
void wii_snapshot_reset();

/**
 * Returns the index of the current snapshot.
 *
 * @param   isLatest (out) Whether the current snapshot index is the latest
 *              snapshot (most recent)
 * @return  The index of the current snapshot
 */
int wii_snapshot_current(BOOL* isLatest);

/**
 * Returns whether the current snapshot exists
 *
 * @return  Whether the current snapshot exists
 */
BOOL wii_snapshot_current_exists();

/**
 * Moves to the next snapshot (next index)
 *
 * @return  The index that was moved to
 */
int wii_snapshot_next();

#endif
