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
#define WII_DB_DIR WII_FILES_DIR "db/"
#define WII_ROMS_DIR WII_FILES_DIR "roms/"
#define WII_SAVES_DIR WII_FILES_DIR "saves/"
#define WII_LANG_DIR WII_FILES_DIR "lang/"
#define WII_CONFIG_FILE WII_FILES_DIR "wiimednafen.conf"

/**
 * The different types of nodes in the menu
 */
enum NODETYPE {
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
    NODETYPE_BUTTON1,
    NODETYPE_BUTTON2,
    NODETYPE_BUTTON3,
    NODETYPE_BUTTON4,
    NODETYPE_BUTTON5,
    NODETYPE_BUTTON6,
    NODETYPE_BUTTON7,
    NODETYPE_BUTTON8,
    NODETYPE_BUTTON9,
    NODETYPE_BUTTON10,
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
    NODETYPE_CONTROL_TYPE1,
    NODETYPE_CONTROL_TYPE2,
    NODETYPE_CONTROL_TYPE3,
    NODETYPE_CONTROL_TYPE4,
    NODETYPE_CHEATS,
    NODETYPE_GAME_GENIE,
    NODETYPE_GBA_BIOS,
    NODETYPE_DOUBLE_STRIKE,
    NODETYPE_DOUBLE_STRIKE_CART,
    NODETYPE_FULL_WIDESCREEN,
    NODETYPE_MD_REGION,
    NODETYPE_TRAP_FILTER,
    NODETYPE_VIDEO_SETTINGS,
    NODETYPE_GX_VI_SCALER_SPACER,
    NODETYPE_GX_VI_SCALER,
    NODETYPE_16_9_CORRECTION,
    NODETYPE_VOLUME,
    NODETYPE_VOLUME_CART,
    NODETYPE_PROFILE,
    NODETYPE_REWIND,
    NODETYPE_REWIND_BUTTON,
    NODETYPE_WIIMOTE_MODE,
    NODETYPE_CENTER_ADJUST,
    NODETYPE_SENSITIVITY,
    NODETYPE_SMOOTHING,
    NODETYPE_EMU_WIIMOTE_MODE,
    NODETYPE_EMU_CENTER_ADJUST,
    NODETYPE_EMU_SENSITIVITY,
    NODETYPE_EMU_SMOOTHING
};

#ifdef __cplusplus
}
#endif

#endif
