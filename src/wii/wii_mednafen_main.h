/*
WiiMednafen : Wii port of the Mednafen emulator

Copyright (C) 2011 raz0red
*/

#ifndef WII_MEDNAFEN_MAIN_H
#define WII_MEDNAFEN_MAIN_H

#include "Emulator.h"

/*
 * Initializes the emulator
 */
extern void wii_mednafen_init();

/*
 * Free resources (closes) the emulator
 */
extern void wii_mednafen_free();

/**
 * Loads the specified game
 *
 * game     The name of the game
 * return   1 if the load is successful, 0 if it fails
 */
extern int wii_mednafen_load_game( char* game );

/*
 * The emulation loop
 *
 * resume   Whether we are resuming
 */
extern void wii_mednafen_emu_loop( BOOL resume );

/*
 * Displays a screen indicating the controls configuration for the current
 * cartridge
 *
 * return   Returns 1 if we should continue past the screen
 */
extern int wii_mednafen_show_controls_screen();

/*
 * Displays a message during emulation
 *
 * message  The message to display
 */
extern void wii_mednafen_set_message( const char* message );

/*
 * Returns the last display rect
 *
 * return   The last display rect
 */
extern Rect* wii_mednafen_get_last_rect();

/*
 * Resets the last cached display rect size
 */
extern void wii_mednafen_reset_last_rect();

/*
 * Whether the controls should be reset prior to entering the emulator loop.
 */
void wii_mednafen_set_reset_controls();

#endif
