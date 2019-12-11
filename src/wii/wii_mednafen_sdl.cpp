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

#include "wii_sdl.h"
#include "wii_mednafen.h"

// The original 8bpp palette
SDL_Palette orig_8bpp_palette;
SDL_Color orig_8bpp_colors[512];

/**
 * Initializes SDL
 */
int wii_sdl_handle_init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        return 0;
    }

    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
        return 0;
    }

    back_surface = SDL_SetVideoMode(
        640, 480, 8, SDL_HWSURFACE | SDL_HWPALETTE | SDL_FULLSCREEN);

    if (!back_surface) {
        return 0;
    }

    int ncolors = back_surface->format->palette->ncolors;
    memcpy(orig_8bpp_colors, back_surface->format->palette->colors,
           ncolors * sizeof(SDL_Color));
    orig_8bpp_palette.colors = orig_8bpp_colors;
    orig_8bpp_palette.ncolors = ncolors;

    return 1;
}
