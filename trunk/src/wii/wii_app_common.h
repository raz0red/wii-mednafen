/*
WiiMednafen : Wii port of the Mednafen emulator

Copyright (C) 2011 raz0red

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

#ifndef WII_APP_COMMON_H
#define WII_APP_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef USB_WIILOAD
#define WII_BASE_APP_DIR "sd:/apps/wiimednafen/"
#else
#define WII_BASE_APP_DIR "usb:/apps/wiimednafen/"
#endif
#define WII_SAVE_GAME_EXT "sav"
#define WII_LANG_EXT "lang"

#define WII_FILES_DIR "/wiimednafen/"
#define WII_ROMS_DIR WII_FILES_DIR "roms/"
#define WII_SAVES_DIR WII_FILES_DIR "saves/"
#define WII_LANG_DIR WII_FILES_DIR "lang/"
#define WII_CONFIG_FILE WII_FILES_DIR "wiimednafen.conf"

/*
 * The different types of nodes in the menu
 */
enum NODETYPE
{
  NODETYPE_ROOT = 0,
  NODETYPE_SPACER,
  NODETYPE_LOAD_ROM,
  NODETYPE_ROOT_DRIVE,
  NODETYPE_UPDIR,
  NODETYPE_DIR,
  NODETYPE_ROM,
  NODETYPE_EXIT,
  NODETYPE_RESUME,
  NODETYPE_ADVANCED,
  NODETYPE_DEBUG_MODE,
  NODETYPE_TOP_MENU_EXIT,
  NODETYPE_RESET,
  NODETYPE_SAVE_STATE,
  NODETYPE_LOAD_STATE,
  NODETYPE_DELETE_STATE,
  NODETYPE_VSYNC,
  NODETYPE_DISPLAY_SETTINGS,
  NODETYPE_CONTROLS_SETTINGS,
  NODETYPE_CARTRIDGE_SETTINGS_CURRENT,
  NODETYPE_CARTRIDGE_SETTINGS_CURRENT_SPACER,
  NODETYPE_CARTRIDGE_SETTINGS_DEFAULT,
  NODETYPE_SAVE_CARTRIDGE_SETTINGS,
  NODETYPE_DELETE_CARTRIDGE_SETTINGS,
  NODETYPE_REVERT_CARTRIDGE_SETTINGS,
  NODETYPE_CONTROLS_MODE,
  NODETYPE_RESIZE_SCREEN,
  NODETYPE_WIIMOTE_MENU_ORIENT,
  NODETYPE_VB_MODE,
  NODETYPE_FRAME_SKIP,
  NODETYPE_CARTRIDGE_SETTINGS_DISPLAY,
  NODETYPE_CARTRIDGE_SETTINGS_CONTROLS,
  NODETYPE_CARTRIDGE_SETTINGS_MAPPINGS,
  NODETYPE_CARTRIDGE_SETTINGS_ADVANCED,
  NODETYPE_CART_FRAME_SKIP,
  NODETYPE_CART_RENDER_RATE,
  NODETYPE_CONTROLLER,
  NODETYPE_WIIMOTE_SUPPORTED,
  NODETYPE_BUTTON1, NODETYPE_BUTTON2, NODETYPE_BUTTON3, NODETYPE_BUTTON4,
  NODETYPE_BUTTON5, NODETYPE_BUTTON6, NODETYPE_BUTTON7, NODETYPE_BUTTON8,
  NODETYPE_BUTTON9, NODETYPE_BUTTON10,
  NODETYPE_ROM_PATCH,
  NODETYPE_ROM_PATCH_CART,
  NODETYPE_SELECT_LANG,
  NODETYPE_LANG_MENU,
  NODETYPE_LANG_DEFAULT,
  NODETYPE_LANG,
  NODETYPE_EMULATOR_SETTINGS_SPACER,
  NODETYPE_EMULATOR_SETTINGS,
  NODETYPE_ORIENT,
  NODETYPE_FILTER,
  NODETYPE_CARTRIDGE_SAVE_STATES_SPACER,
  NODETYPE_CARTRIDGE_SAVE_STATES,
  NODETYPE_CARTRIDGE_SAVE_STATES_SLOT,
  NODETYPE_AUTO_LOAD_SAVE,
  NODETYPE_GAME_LANGUAGE,
  NODETYPE_CONTROL_TYPE1, NODETYPE_CONTROL_TYPE2, NODETYPE_CONTROL_TYPE3,
  NODETYPE_CONTROL_TYPE4,
  NODETYPE_CHEATS
};

#ifdef __cplusplus
}
#endif

#endif
