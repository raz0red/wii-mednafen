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

// The last cartridge hash
extern char wii_cartridge_hash[33];
// The cartridge hash with header (may be the same)
extern char wii_cartridge_hash_with_header[33];
// Whether to display debug info (FPS, etc.)
extern BOOL wii_debug;
// Hardware buttons (reset, power, etc.)
extern u8 wii_hw_button;
// Auto load state?
extern BOOL wii_auto_load_state;
// Auto save state?
extern BOOL wii_auto_save_state;
// The current language
extern char wii_language[WII_MAX_PATH];

/*
 * Returns the roms directory
 *
 * return   The roms directory
 */
extern char* wii_get_roms_dir();

/*
 * Returns the saves directory
 *
 * return   The saves directory
 */
extern char* wii_get_saves_dir();

/*
 * Returns the lang directory
 *
 * return   The roms directory
 */
extern char* wii_get_lang_dir();

/*
 * Returns the base directory
 *
 * return   The bae directory
 */
extern char* wii_get_base_dir();

#endif
