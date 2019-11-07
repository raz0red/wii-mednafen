/*
WiiMednafen : Wii port of the Mednafen emulator

Copyright (C) 2011 raz0red
*/

#ifndef WII_MEDNAFEN_SNAPSHOT_H
#define WII_MEDNAFEN_SNAPSHOT_H

#include <gccore.h>

/*
 * Starts the emulator for the specified snapshot file.
 *
 * savefile The name of the save file to load. 
 */
extern BOOL wii_start_snapshot();

extern void wii_snapshot_reset();
extern int wii_snapshot_current( BOOL* isLatest );
extern BOOL wii_snapshot_current_exists();
extern int wii_snapshot_next();

#endif
