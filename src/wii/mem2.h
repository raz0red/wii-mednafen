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
|  This program is free software; you can redistribute it and/or             |
|  modify it under the terms of the GNU General Public License               |
|  as published by the Free Software Foundation; either version 2            |
|  of the License, or (at your option) any later version.                    |
|                                                                            |
|  This program is distributed in the hope that it will be useful,           |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of            |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             |
|  GNU General Public License for more details.                              |
|                                                                            |
|  You should have received a copy of the GNU General Public License         |
|  along with this program; if not, write to the Free Software               |
|  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA             |
|  02110-1301, USA.                                                          |
|                                                                            |
\*--------------------------------------------------------------------------*/

#ifndef MEM2_H
#define MEM2_H

#include <gctypes.h>

extern void InitMem2Manager();
extern void Mem2ManagerReset();
extern u8* Mem2ManagerAlloc( u32 size, const char* purpose );
extern u8* Mem2ManagerCalloc( int count, u32 size, const char* purpose );
extern u8* Mem2ManagerAdjust( u8* mem, u32 size, const char* purpose );

#endif