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

#include "main.h"
#include "wii_snapshot.h"
#include "../../mednafen/src/general.h"

#include <stdio.h>

#include "wii_app.h"
#include "wii_util.h"

#include "wii_mednafen.h"
#include "wii_mednafen_emulation.h"
#include "wii_mednafen_main.h"

// Mednafen externs
extern volatile MDFN_Surface *VTReady;
extern volatile MDFN_Rect *VTLWReady;
extern volatile MDFN_Rect *VTDRReady;

/** Status of current saved state */
static StateStatusStruct* stateStatus = NULL;

/**
 * Resets snapshot related information. This method is typically invoked when
 * a new rom file is loaded.
 */
void wii_snapshot_reset() {
    if (stateStatus != NULL) {
        StateStatusStruct* ss = stateStatus;
        stateStatus = NULL;
        if (ss->gfx != NULL) {
            free(ss->gfx);
        }
        free(ss);
    }
}

/**
 * Returns the index of the current snapshot.
 *
 * @param   isLatest (out) Whether the current snapshot index is the latest
 *              snapshot (most recent)
 * @return  The index of the current snapshot
 */
int wii_snapshot_current(BOOL* isLatest) {
    *isLatest = FALSE;
    if (stateStatus == NULL) {
        stateStatus = MDFNI_SelectState(0);
    }

    if (stateStatus != NULL) {
        int curr = stateStatus->current;
        *isLatest = ((curr == stateStatus->recently_saved) &&
                     stateStatus->status[stateStatus->current]);
        return curr;
    } else {
        return 0;
    }
}

/**
 * Returns whether the current snapshot exists
 *
 * @return  Whether the current snapshot exists
 */
BOOL wii_snapshot_current_exists() {
    return stateStatus != NULL && stateStatus->status[stateStatus->current];
}

/**
 * Refreshes current snapshot state
 */
static void refresh() {
    if (stateStatus != NULL) {
        int index = stateStatus->current;
        wii_snapshot_reset();
        stateStatus = MDFNI_SelectState(index);
    }
}

/**
 * Moves to the next snapshot (next index)
 *
 * @return  The index that was moved to
 */
int wii_snapshot_next() {
    int nextState = stateStatus != NULL ? (stateStatus->current + 1) : 0;

    if (nextState == 10) {
        nextState = 0;
    }

    wii_snapshot_reset();
    stateStatus = MDFNI_SelectState(nextState);

    return nextState;
}

/**
 * Returns the name of the snapshot associated with the specified romfile and
 * the current snapshot index
 *
 * @param   romfile The rom file
 * @param   buffer The output buffer to receive the name of the snapshot file
 *              (length must be WII_MAX_PATH)
 */
void wii_snapshot_handle_get_name(const char* romfile, char* buffer) {
    Util_strlcpy(
        buffer,
        MDFN_MakeFName(MDFNMKF_STATE, stateStatus->current, NULL).c_str(),
        WII_MAX_PATH);
}

/**
 * Attempts to save the snapshot to the specified file name
 *
 * @param   The name to save the snapshot to
 * @return  Whether the snapshot was successful
 */
BOOL wii_snapshot_handle_save(char* filename) {
    BOOL success = MDFNI_SaveState(filename, NULL, (const MDFN_Surface*)VTReady,
                                   (const MDFN_Rect*)VTDRReady,
                                   (const MDFN_Rect*)VTLWReady);
    if (success) {
        refresh();
    }

    return success;
}

/**
 * Starts emulation with the current snapshot
 *
 * @return  Whether emulation was successfully started
 */
BOOL wii_start_snapshot() {
    BOOL succeeded = MDFNI_LoadState(NULL, NULL);
    if (!succeeded) {
        wii_set_status_message("Error loading the specified save state file.");
    } else {
        wii_resume_emulation();
    }

    return TRUE;
}