<?php require("docgen.inc"); ?>

<?php BeginPage('', 'General'); ?>

 <?php BeginSection("Introduction"); ?>
 <p>
 This main document covers general Mednafen usage, generally regardless of which system is being emulated.  Documentation covering key assignments, settings, and related information for each system emulation module is linked to in the table of contents under "Emulation Module Documentation".
 </p>

 <p>
  The term "movie" used in this documentation refers to save-state and recorded input stream stored in a file, generally usable only in Mednafen itself.<br>
  The terms "audio/video movie", "QuickTime movie", and variations thereof refer to audio and video data recorded and stored in a file, and
  usable with external programs.
 </p>
  <?php BeginSection("Base Directory"); ?>
   <p>
    Mednafen's "base directory" is the directory under which Mednafen stores its data and
    looks for various auxillary data by default.  If the "HOME" environment variable is set, it will be suffixed with "/.mednafen"
    and used as the base directory(in other words ~/.mednafen, or $HOME/.mednafen).  If the "HOME" environment
    variable is not set, and the current user has an entry in the password file, the corresponding home directory
    will be used as if it were the "HOME" environment variable.
   </p>

   <p>
    On Microsoft Windows, these conditions are typically
    not met, in which case the directory the Mednafen executable("mednafen.exe") is in will be used as the base directory.
   </p>

   <p>
    If none of the preceding conditions were met, then you're doomed, doomy doomy DOOMED, GOTO DOOM.
   </p>

  <?php EndSection(); ?>

 <?php EndSection(); ?>

 <?php BeginSection("Core Features"); ?>
 <ul>
  <li>Physical joystick/gamepad support.</li>
  <li>Extremely versatile input configuration system; assign multiple physical buttons to a virtual button or action.</li>
  <li>Various graphics filters and scaling modes.</li>
  <li>Save states.</li>
  <li>Real-time game rewinding.</li>
  <li>Screen snapshots, saved in PNG format.</li>
  <li>QuickTime movie recording.</li>
  <li>MS WAV-format sound logging.</li>
  <li>Loading games from gzip and (pk)zip compressed archives.</li>
  <li>Network play(utilizing an external dedicated server program).</li>
</ul>

 <?php BeginSection("CD-ROM Emulation"); ?>
 <p>
  With PC Engine CD and PC-FX emulation, Mednafen can load CD-ROM games from a physical CD-ROM, or a ripped/dumped copy of the disc, such as CUE+BIN.
  Using a ripped copy of the disc is recommended, due to speed issues and to the fact that physical CDs will be missing pregaps from the point
  of view of the emulated system.
 </p>
 <p>
  For ripped disc images, Mednafen supports "CUE" sheets and cdrdao "TOC" files.  Mednafen supports raw, simple storage formats supported by
  libsndfile(MS WAV, AIFF/AIFC, AU/SND, etc.), Ogg Vorbis, and MusePack audio files referenced by CUE sheets.  MP3 is not supported, and will not be 
  supported.
 </p>
 <p>
  The cdrdao "TOC" support in Mednafen includes support for "RW_RAW" subchannel data, needed for CD+G.  Note that Mednafen assumes that the Q subchannel
  is also included in the RW_RAW data area in the ripped image(even though the name "RW_RAW" would suggest it isn't present, cdrdao seems to included it).  If
  the Q subchannel data is missing from the RW_RAW data area in the ripped image, Mednafen's CD emulation will not work properly.
 </p>
 <p>
  Since 0.8.4, Mednafen will perform simple data correction on ripped CDROM images that contain EDC and L-EC data(2352-byte-per-sector "raw" rips").
  It calculates the real EDC, and if it doesn't match the EDC recorded for that sector, it will evaluate the L-EC data to repair the data.  If the
  data is unrepairable, an error message will be displayed.
  <br>
  <b>This may cause problems with naive patches that don't update the error-correction data(at least the 32-bit EDC, if that's correct, the L-EC data will
  be ignored)!</b>  If you wish to use such an invalid patch, set the setting "cdrom.lec_eval" to 0.
 </p>
 <?php EndSection(); ?>

 <?php EndSection(); ?>

 <?php BeginSection("Using Mednafen"); ?>
 <p>
 
 </p>
 <?php BeginSection("Key Assignments"); ?>
 <p>
 <table border>
 <tr><th>Key(s):</th><th>Action:</th><th>Configuration String:</th></tr>
 <tr><td>F1</td><td>Toggle in-game quick help screen.</td><td>toggle_help</td></tr>
 <tr><td>F5</td><td>Save state.</td><td>save_state</td></tr>
 <tr><td>F7</td><td>Load state.</td><td>load_state</td></tr>
 <tr><td>0-9</td><td>Select save state slot.</td><td>"0" through "9"</td></tr>
 <tr><td>-</td><td>Decrement selected save state slot.</td><td>state_slot_dec</td></tr>
 <tr><td>=</td><td>Increment selected save state slot.</td><td>state_slot_inc</td></tr>
 <tr><td>Shift + F5</td><td>Record movie.</td><td>save_movie</td></tr>
 <tr><td>Shift + F7</td><td>Play movie.</td><td>load_movie</td></tr>
 <tr><td>Shift + 0-9</td><td>Select movie slot.</td><td>"m0" through "m9"</td></tr>
 <tr><td>ALT + C</td><td>Toggle cheat console.</td><td>togglecheatview</td></tr>
 <tr><td>ALT + T</td><td>Toggle cheats active.</td><td>togglecheatactive</td></tr>
 <tr><td>T</td><td>Enable network play console input.</td><td>togglenetview</td></tr>
 <tr><td>ALT + D</td><td>Toggle debugger.</td><td>toggle_debugger</td></tr>
 <tr><td>ALT + A</td><td>Enter frame advance mode, if not already in it, and advance the frame.</td><td>advance_frame</td></tr>
 <tr><td>ALT + R</td><td>Exit frame advance mode.</td><td>run_normal</td></tr>
 <tr><td>SHIFT + F1</td><td>Toggle frames-per-second display(from top to bottom, the display format is: virtual, rendered, blitted).</td><td>toggle_fps_view</td></tr>
 <tr><td>ALT + S</td><td>Toggle 600-frame(~10 seconds on NES) save-state rewinding functionality, disabled by default.</td><td>state_rewind_toggle</td></tr>
 <tr><td>Backspace</td><td>Rewind emulation, if save-state rewinding functionality is enabled, up to 600 frames.</td><td>state_rewind</td></tr>
 <tr><td>F9</td><td>Save screen snapshot.</td><td>take_snapshot</td></tr>
 <tr><td>Alt + Enter</td><td>Toggle fullscreen mode.</td><td>toggle_fs</td></tr>
 <tr><td nowrap>Ctrl + 1<br>through<br>Ctrl + 9</td><td>Toggle layer.</td><td>"tl1" through "tl9"</tr>
 <tr><td>~</td><td>Fast-forward.</td><td>fast_forward</td></tr>
 <tr><td>CTRL + SHIFT + <i>i</i></td><td>Select input device on input port <i>i</i>(1-5).<br><b>Note:</b>Many games do not expect input devices to change while the game is running, and thus may require a hard reset.</td><td>device_select<i>i</i></td></tr>
 <tr><td>F2</td><td>Activate in-game input configuration process for a command key.</td><td>input_configc</td></tr>
 <tr><td nowrap>Scroll Lock</td><td>Toggle input grabbing(use this to use the PC Engine mouse properly in windowed mode).</td><td>toggle_grab_input</td></tr>
 <tr><td>Shift + Scroll Lock</td><td>Toggle input grabbing AND command disabling(used for giving almost unhindered button access to an emulated device; most useful when emulating the Famicom's Family Keyboard).</td><td>toggle_cdisable</td></tr>
 <tr><td>F10</td><td>Reset.</td><td>reset</td></tr>
 <tr><td>F11</td><td>Hard reset(toggle power switch).</td><td>power</td></tr>
 <tr><td>Escape/F12</td><td>Exit(the emulator, or netplay chat mode).</td><td>exit</td></tr>
 </table>
 </p>

 <?php EndSection(); ?>

 <?php BeginSection("Command-line"); ?>
 <p>
  Mednafen supports arguments passed on the command line.   Arguments
  are taken in the form of "-parameter value".  Some arguments are valueless.
 </p>
 <p>
  In addition to the arguments listed in the table below, any setting listed in the "Settings" section of this document and any system emulation module sub-document can be set by prefixing it with
  a hyphen(-), followed by the parameter, such as: -nes.slstart 8
</p>
  <table border>
   <tr><th>Argument:</th><th>Parameter Type:</th><th>Description:</th></tr>
   <tr><td>-loadcd x</td><td>string</td><td>Load and boot a CD for system "x"(only "pce" is valid).  This argument modifies the usage of the filename component of the command-line.  For example, "mednafen -loadcd pce /somewhere/over/the/rainbow/game.cue" will load the CUE sheet as the emulated CDROM.
"mednafen -loadcd pce" will load from the default physical CDROM device. </td></tr>
   <tr><td nowrap>-force_module x</td><td>string</td><td>Force usage of specified emulation module.</td></tr>

   <tr><td>-connect</td><td></td><td>Trigger to connect to remote host after the game is loaded.</td></tr>
   <tr><td nowrap>-soundrecord x</td><td>string</td><td>Record sound output to the specified filename in the MS WAV format.</td></tr>
   <tr><td nowrap>-qtrecord x</td><td>string</td><td>Record video and audio output to the specified filename in the QuickTime format.</td></tr>
  </table>
 <?php EndSection(); ?>

<?php PrintSettings("Global Settings Reference"); ?>

 <?php BeginSection("Firmware/BIOS"); ?>
<p>
Some emulation modules require firmware/BIOS images to function.  If a firmware path is non-absolute(doesn't begin with
C:\ or / or similar), Mednafen will try to load the file relative to the "firmware" directory under the Mednafen base
directory.  If it doesn't find it there, it will be loaded relative to the Mednafen base directory itself.  Of course,
if the "path_firmware" setting is set to a custom value, the firmware files will be searched relative to that path.
</p>
 <?php EndSection(); ?>

 <?php BeginSection("Custom Palettes"); ?>
<p>
Custom palettes for a system should be named <sysname>.pal, IE "nes.pal", "pce.pal", etc., and placed in the
"palettes" directory beneath the Mednafen base directory.
</p>
<p>
Per-game custom palettes are also supported, and should be
named as &lt;FileBase&gt;.pal or &lt;FileBase&gt;.&lt;MD5 Hash&gt;.pal, IE "Mega Man 4.pal" or "Mega Man 4.db45eb9413964295adb8d1da961807cc.pal".
</p>
<p>
Not all emulated systems support custom palettes.  Currently, the following emulation modules support custom palettes:
<ul>
 <li>gba</li>
 <li>nes</li>
 <li>pce</li>
 <li>pce_fast</li>
</ul>
</p>
 <?php EndSection(); ?>

 <?php BeginSection("Automatic IPS Patching"); ?>
 <p>
        Place the IPS file in the same directory as the file to load,
        and name it &lt;FullFileName&gt;.ips.
        <pre>
        Examples:       Boat.nes - Boat.nes.ips
                        Boat.zip - Boat.zip.ips
                        Boat.nes.gz - Boat.nes.gz.ips
                        Boat     - Boat.ips
        </pre>
 </p>
 <p>
  Some operating systems and environments will hide file extensions. Keep this in mind if you are having trouble.
 </p>
 <p>
        Patching is applied in a file format-agnostic way; however, dynamic patching is not done with CD images, nor with
	firmware.
 </p>
 <?php EndSection(); ?>
 
 <?php EndSection(); ?>

 <?php DoModDocLinks(); ?>

 <?php BeginSection("Cheat Guide", "", "cheat.html"); ?> <?php EndSection(); ?>
 <?php BeginSection("Debugger", "", "debugger.html"); ?> <?php EndSection(); ?>
 <?php BeginSection("Network Play", "", "netplay.html"); ?> <?php EndSection(); ?>


 <?php BeginSection("Credits"); ?>
 <p>
  (This section is woefully outdated, being mostly copied from FCE Ultra)
 <table border width="100%">
  <tr><th>Name:</th><th>Contribution(s):</th></tr>
  <tr><td>\Firebug\</td><td>High-level mapper information.</td></tr>
  <tr><td>Bero</td><td>Original FCE source code.</td></tr>
  <tr><td>Brad Taylor</td><td>NES sound information.</td></tr>
  <tr><td>Charles MacDonald</td><td>PC Engine technical information.</td></tr>
  <tr><td>EFX</td><td>Testing.</td></tr>
  <tr><td>Fredrik Olson</td><td>NES four-player adapter information.</td></tr>
  <tr><td>goroh</td><td>Various documents.</td></tr>
  <tr><td>Jeremy Chadwick</td><td>General NES information.</td></tr>
  <tr><td>kevtris</td><td>Low-level NES information and sound information.</td></tr>
  <tr><td>Ki</td><td>Various technical information.</td></tr>
  <tr><td>Mark Knibbs</td><td>Various NES information.</td></tr>
  <tr><td>Marat Fayzullin</td><td>General NES information.</td></tr>
  <tr><td>Matthew Conte</td><td>Sound information.</td></tr>
  <tr><td>nori</td><td>FDS sound information.</td></tr>
  <tr><td>rahga</td><td>Famicom four-player adapter information.</td></tr>
  <tr><td>TheRedEye</td><td>ROM images, testing.</td></tr>
  <tr><th colspan="2" align="right">...and everyone whose name my mind has misplaced.</th></tr>
 </table>
 </p>
 <?php EndSection(); ?>

 <?php BeginSection("Licenses, Copyright Notices, and Code Credits"); ?>
 <p>
  Mednafen makes use of much open-source code from other people, and could not be what it is without their work.  Feel
  free to give them your thanks, but keep in mind most have nothing to do with the Mednafen project, so don't ask
  them questions regarding Mednafen unless appropriate in context.
 </p>
 <p>
 In addition to the listing of licenses and copyright notices for code included in Mednafen, the following 
 "non-system" external libraries are linked to:
 <ul>
  <li><a href="http://www.gnu.org/software/libcdio/">libcdio</a></li>
  <li><a href="http://www.mega-nerd.com/libsndfile/">libsndfile</a></li>
  <li><a href="http://www.libsdl.org/">SDL</a></li>
  <li><a href="http://www.libsdl.org/projects/SDL_net/">SDL_net</a></li>
  <li><a href="http://www.zlib.org">zlib</a></li>
 </ul>
 </p>
 <hr>

 <p>
  <?php BeginSection("libmpcdec", "", FALSE, "http://www.musepack.net/"); ?>
  <blockquote>
  <pre>
  Copyright (c) 2005, The Musepack Development Team
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

  * Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the following
  disclaimer in the documentation and/or other materials provided
  with the distribution.

  * Neither the name of the The Musepack Development Team nor the
  names of its contributors may be used to endorse or promote
  products derived from this software without specific prior
  written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  </pre>
  </blockquote>
  <?php EndSection(); ?>

  <?php BeginSection("Tremor", "", FALSE, "http://xiph.org/"); ?>
  <blockquote>
<pre>
Copyright (c) 2002, Xiph.org Foundation

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

- Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

- Neither the name of the Xiph.org Foundation nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
</pre>
  </blockquote>
  <?php EndSection(); ?>

 <?php BeginSection("Gb_Snd_Emu", "", FALSE, "http://slack.net/~ant/libs/audio.html"); ?>
<blockquote>
<pre>
/* Library Copyright (C) 2003-2004 Shay Green. Gb_Snd_Emu is free
software; you can redistribute it and/or modify it under the terms of the
GNU General Public License as published by the Free Software Foundation;
either version 2 of the License, or (at your option) any later version.
Gb_Snd_Emu is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
more details. You should have received a copy of the GNU General Public
License along with Gb_Snd_Emu; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA */
</pre>
</blockquote>
 <?php EndSection(); ?>

<?php BeginSection("Blip_Buffer", "", FALSE, "http://www.slack.net/~ant/libs/"); ?>
<blockquote>
<pre>
Copyright (C) 2003-2006 Shay Green. This module is free software; you
can redistribute it and/or modify it under the terms of the GNU Lesser
General Public License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version. This
module is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details. You should have received a copy of the GNU Lesser General
Public License along with this module; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
</pre>
</blockquote>
 <?php EndSection(); ?>

 <?php BeginSection("Sms_Snd_Emu(base for T6W28_Apu NGP code)", "", FALSE, "http://slack.net/~ant/libs/audio.html"); ?>
<blockquote>
<pre>
Copyright (C) 2003-2006 Shay Green. This module is free software; you
can redistribute it and/or modify it under the terms of the GNU Lesser
General Public License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version. This
module is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details. You should have received a copy of the GNU Lesser General
Public License along with this module; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
</pre>
</blockquote>
 <?php EndSection(); ?>

<?php BeginSection("Handy", "", FALSE, "http://homepage.ntlworld.com/dystopia/"); ?>
<blockquote>
<pre>
Copyright (c) 2004 K. Wilkins

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from
the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not
   be misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.
</pre>
</blockquote>
 <?php EndSection(); ?>

 <?php BeginSection("PC2e (Used in portions of PC Engine CD emulation)"); ?>
<blockquote>
<pre>
        Copyright (C) 2004 Ki

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
</pre>
</blockquote>
 <?php EndSection(); ?>

 <?php BeginSection("Scale2x", "", FALSE, "http://scale2x.sf.net/"); ?>
<blockquote>
<pre>
 * Copyright (C) 2001, 2002, 2003, 2004 Andrea Mazzoleni
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
</pre>
</blockquote>
 <?php EndSection(); ?>

<?php BeginSection("hq2x, hq3x, hq4x", "", FALSE, "http://www.hiend3d.com/hq2x.html"); ?>
<blockquote>
<pre>
//Copyright (C) 2003 MaxSt ( maxst@hiend3d.com )

//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU Lesser General Public
//License as published by the Free Software Foundation; either
//version 2.1 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
//Lesser General Public License for more details.
//
//You should have received a copy of the GNU Lesser General Public
//License along with this program; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
</pre>
</blockquote>
 <?php EndSection(); ?>

<?php BeginSection("nes_ntsc", "", FALSE, "http://www.slack.net/~ant/libs/ntsc.html#nes_ntsc"); ?>
<blockquote>
<pre>
/* Copyright (C) 2006 Shay Green. This module is free software; you
can redistribute it and/or modify it under the terms of the GNU Lesser
General Public License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version. This
module is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details. You should have received a copy of the GNU Lesser General
Public License along with this module; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA */
</pre>
</blockquote>
 <?php EndSection(); ?>

<?php BeginSection("2xSaI"); ?>
<blockquote>
<pre>
/* 2xSaI
 * Copyright (c) Derek Liauw Kie Fa, 1999-2002
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/* http://lists.fedoraproject.org/pipermail/legal/2009-October/000928.html */
</pre>
</blockquote>
<?php EndSection(); ?>

<?php BeginSection("VisualBoyAdvance GameBoy and GBA code", "", FALSE, "http://vba.sf.net/"); ?>
<blockquote>
<pre>
// VisualBoyAdvance - Nintendo Gameboy/GameboyAdvance (TM) emulator.
// Copyright (C) 1999-2003 Forgotten
// Copyright (C) 2004-2006 Forgotten and the VBA development team

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or(at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
</pre>
</blockquote>
 <?php EndSection(); ?>

<?php BeginSection("trio", "", FALSE, "http://daniel.haxx.se/projects/trio/"); ?>
<blockquote>
<pre>
 * Copyright (C) 1998, 2009 Bjorn Reese and Daniel Stenberg.
 * Copyright (C) 2001 Bjorn Reese <breese@users.sourceforge.net>
 * Copyright (C) 2001 Bjorn Reese and Daniel Stenberg.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE AUTHORS AND
 * CONTRIBUTORS ACCEPT NO RESPONSIBILITY IN ANY CONCEIVABLE MANNER.
 *
</pre>
</blockquote>

<?php EndSection(); ?>

<?php BeginSection("ConvertUTF"); ?>
<blockquote>
<pre>
 * Copyright 2001-2004 Unicode, Inc.
 *
 * Disclaimer
 *
 * This source code is provided as is by Unicode, Inc. No claims are
 * made as to fitness for any particular purpose. No warranties of any
 * kind are expressed or implied. The recipient agrees to determine
 * applicability of information provided. If this file has been
 * purchased on magnetic or optical media from Unicode, Inc., the
 * sole remedy for any claim will be exchange of defective media
 * within 90 days of receipt.
 *
 * Limitations on Rights to Redistribute This Code
 *
 * Unicode, Inc. hereby grants the right to freely use the information
 * supplied in this file in the creation of products supporting the
 * Unicode Standard, and to make copies of this file in any form
 * for internal or external distribution as long as this notice
 * remains attached.
</pre>
</blockquote>
<?php EndSection(); ?>

<?php BeginSection("MD5 Hashing"); ?>
<blockquote>
<pre>
/*
 * RFC 1321 compliant MD5 implementation,
 * by Christophe Devine <devine@cr0.net>;
 * this program is licensed under the GPL.
 */
</pre>
</blockquote>
 <?php EndSection(); ?>

<?php BeginSection("V810 Emulator"); ?>
<blockquote>
<pre>
 * Copyright (C) 2006 David Tucker
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
</pre>
</blockquote>
<?php EndSection(); ?>

<?php BeginSection("Unzipping Code"); ?>
<blockquote>
<pre>
/* unzip.c -- IO for uncompress .zip files using zlib
   Version 1.01e, February 12th, 2005

   Copyright (C) 1998-2005 Gilles Vollant

   Read unzip.h for more info
*/

/* Decryption code comes from crypt.c by Info-ZIP but has been greatly reduced in terms of
compatibility with older software. The following is from the original crypt.c. Code
woven in by Terry Thorsen 1/2003.
*/
/*
  Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
</pre>
</blockquote>

<?php EndSection(); ?>

<?php BeginSection("MiniLZO", "", FALSE, "http://www.oberhumer.com/opensource/lzo/"); ?>
<blockquote>
<pre>
   Copyright (C) 2005 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 2004 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 2003 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 2002 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 2001 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 2000 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1999 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1998 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1997 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1996 Markus Franz Xaver Johannes Oberhumer
   All Rights Reserved.

   The LZO library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The LZO library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the LZO library; see the file COPYING.
   If not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   Markus F.X.J. Oberhumer
   &lt;markus@oberhumer.com&gt;
   http://www.oberhumer.com/opensource/lzo/
</pre>
</blockquote>
<?php EndSection(); ?>

<?php BeginSection("Fuse Z80 emulation code", "", FALSE, "http://fuse-emulator.sourceforge.net/"); ?>
<blockquote>
<pre>
   Copyright (c) 1999-2004 Philip Kendall, Witold Filipczyk

   $Id: z80_ops.c,v 1.56 2004/06/09 10:55:09 pak21 Exp $

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

   Author contact information:

   E-mail: pak21-fuse@srcf.ucam.org
   Postal address: 15 Crescent Road, Wokingham, Berks, RG40 2DB, England
</pre>
</blockquote>
<?php EndSection(); ?>

<?php BeginSection("NeoPop Neo Geo Pocket (Color) Code", "", FALSE, "http://neopop.emuxhaven.net/"); ?>
<blockquote>
<pre>
//---------------------------------------------------------------------------
// NEOPOP : Emulator as in Dreamland
//
// Copyright (c) 2001-2002 by neopop_uk
//---------------------------------------------------------------------------
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version. See also the license.txt file for
//      additional informations.
</pre>
</blockquote>
<?php EndSection(); ?>

<?php BeginSection("SoftFloat (Used in V810 FPU Emulation)"); ?>
<blockquote>
<pre>
/*============================================================================

This C source file is part of the SoftFloat IEC/IEEE Floating-point Arithmetic
Package, Release 2b.

Written by John R. Hauser.  This work was made possible in part by the
International Computer Science Institute, located at Suite 600, 1947 Center
Street, Berkeley, California 94704.  Funding was partially provided by the
National Science Foundation under grant MIP-9311980.  The original version
of this code was written as part of a project to build a fixed-point vector
processor in collaboration with the University of California at Berkeley,
overseen by Profs. Nelson Morgan and John Wawrzynek.  More information
is available through the Web page `http://www.cs.berkeley.edu/~jhauser/
arithmetic/SoftFloat.html'.

THIS SOFTWARE IS DISTRIBUTED AS IS, FOR FREE.  Although reasonable effort has
been made to avoid it, THIS SOFTWARE MAY CONTAIN FAULTS THAT WILL AT TIMES
RESULT IN INCORRECT BEHAVIOR.  USE OF THIS SOFTWARE IS RESTRICTED TO PERSONS
AND ORGANIZATIONS WHO CAN AND WILL TAKE FULL RESPONSIBILITY FOR ALL LOSSES,
COSTS, OR OTHER PROBLEMS THEY INCUR DUE TO THE SOFTWARE, AND WHO FURTHERMORE
EFFECTIVELY INDEMNIFY JOHN HAUSER AND THE INTERNATIONAL COMPUTER SCIENCE
INSTITUTE (possibly via similar legal warning) AGAINST ALL LOSSES, COSTS, OR
OTHER PROBLEMS INCURRED BY THEIR CUSTOMERS AND CLIENTS DUE TO THE SOFTWARE.

Derivative works are acceptable, even for commercial purposes, so long as
(1) the source code for the derivative work includes prominent notice that
the work is derivative, and (2) the source code includes prominent notice with
these four paragraphs for those parts of this code that are retained.

=============================================================================*/
</pre>
</blockquote>
<?php EndSection(); ?>

<?php BeginSection("PC-FX MJPEG Decoding"); ?>
<blockquote>
<pre>
/*
 * jrevdct.c
 *
 * Copyright (C) 1991, 1992, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 */
<i>(The file is included in the Mednafen source distribution as mednafen/Documentation/README.jpeg4a)</i>
</pre>
</blockquote>
<?php EndSection(); ?>

<?php BeginSection("VRC7 Sound Emulation"); ?>
<blockquote>
<pre>
/*
YM2413 emulator written by Mitsutaka Okazaki 2001

Permission is granted to anyone to use this software for any purpose,
including commercial applications. To alter this software and redistribute it freely,
if the origin of this software is not misrepresented.
*/
</pre>
</blockquote>
<?php EndSection(); ?>

<?php BeginSection("NEC V30MZ Emulator"); ?>
<blockquote>
<pre>
/* This NEC V30MZ emulator may be used for purposes both commercial and noncommercial if you give the author, Bryan McPhail,
   a small credit somewhere(such as in the documentation for an executable package).
*/
</pre>
</blockquote>
<?php EndSection(); ?>

<?php BeginSection("NEC V30MZ disassembler(modified BOCHS x86 disassembler)"); ?>
<blockquote>
<i>Caution:  Bochs' code is under the LGPL, but it is unclear if "or (at your option) any later version." applies to the x86 disassembler code.</i>
<pre>
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.
 
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
 
You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
</pre>
</blockquote>
<?php EndSection(); ?>

<?php BeginSection("Cygne", "", FALSE, "http://http://cygne.emuunlim.com/"); ?>
<p>
Cygne is distributed under the terms of the GNU GPL Version 2, 1991.<br>Copyright 2002 Dox, dox@space.pl.
</p>
<?php EndSection(); ?>

<?php BeginSection("FCE Ultra", "", FALSE, "http://fceultra.sourceforge.net/"); ?>
<blockquote>
<pre>
/* FCE Ultra - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 1998 BERO
 *  Copyright (C) 2002 Xodnizel
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
</pre>
</blockquote>
<?php EndSection(); ?>

<?php BeginSection("QuickLZ", "", FALSE, "http://www.quicklz.com/"); ?>
<blockquote>
<pre>
// QuickLZ data compression library
// Copyright (C) 2006-2008 Lasse Mikkel Reinhold
// lar@quicklz.com
//
// QuickLZ can be used for free under the GPL-1 or GPL-2 license (where anything 
// released into public must be open source) or under a commercial license if such 
// has been acquired (see http://www.quicklz.com/order.html). The commercial license 
// does not cover derived or ported versions created by third parties under GPL.
</pre>
</blockquote>
<?php EndSection(); ?>

<?php BeginSection("Q-Subchannel CRC16 Code", "", FALSE, "http://cdrdao.sourceforge.net/"); ?>
<blockquote>
<pre>
/*  cdrdao - write audio CD-Rs in disc-at-once mode
 *
 *  Copyright (C) 1998  Andreas Mueller <mueller@daneb.ping.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
</pre>
</blockquote>

<?php EndSection(); ?>

<?php BeginSection("SMS Plus", "", FALSE, "http://cgfm2.emuviews.com/"); ?>
<blockquote>
<pre>
    Copyright (C) 1998-2004  Charles MacDonald

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
</pre>
</blockquote>
<?php EndSection(); ?>

<?php BeginSection("Genesis Plus", "", FALSE, "http://cgfm2.emuviews.com/"); ?>
<blockquote>
<pre>
/*
    Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003  Charles Mac Donald

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
</pre>
</blockquote>
<?php EndSection(); ?>

<?php BeginSection("Genesis Plus GX"); ?>
<blockquote>
<prE>
 *
 *  Copyright (C) 2007 EkeEke
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
</pre>
</blockquote>
<?php EndSection(); ?>

<?php BeginSection("EMU2413(used in SMS emulation)"); ?>
<blockquote>
<pre>
  Copyright (C) Mitsutaka Okazaki 2004

  This software is provided 'as-is', without any express or implied warranty.
  In no event will the authors be held liable for any damages arising from
  the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not
     be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
</pre>
</blockquote>
<?php EndSection(); ?>

<?php BeginSection("blz"); ?>
<blockquote>
<pre>
/* Copyright (C) 2007 Shay Green. This module is free software; you can
redistribute it and/or modify it under the terms of the GNU General Public
License, version 2 or later, as published by the Free Software Foundation.
This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
details. You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */
</pre>
</blockquote>
<?php EndSection(); ?>

<?php BeginSection("CD-ROM data correction code"); ?>
<blockquote>
<pre>
/*  dvdisaster: Additional error correction for optical media.
 *  Copyright (C) 2004-2007 Carsten Gnoerlich.
 *  Project home page: http://www.dvdisaster.com
 *  Email: carsten@dvdisaster.com  -or-  cgnoerlich@fsfe.org
 *
 *  The Reed-Solomon error correction draws a lot of inspiration - and even code -
 *  from Phil Karn's excellent Reed-Solomon library: http://www.ka9q.net/code/fec/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA,
 *  or direct your browser at http://www.gnu.org.
 */
</pre>
</blockquote>
<?php EndSection(); ?>

<?php BeginSection("C68K"); ?>
<blockquote>
<pre>
/*  Copyright 2003-2004 Stephane Dallongeville

    This file is part of Yabause.

    Yabause is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Yabause is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Yabause; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
</pre>
</blockquote>
<?php EndSection(); ?>

<?php BeginSection("YM2612 Emulator"); ?>
<blockquote>
<pre>
/* Copyright (C) 2002 St�phane Dallongeville (gens AT consolemul.com) */
/* Copyright (C) 2004-2006 Shay Green. This module is free software; you
can redistribute it and/or modify it under the terms of the GNU Lesser
General Public License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version. This
module is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details. You should have received a copy of the GNU Lesser General Public
License along with this module; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA */
</pre>
</blockquote>
<?php EndSection(); ?>

 <?php EndSection(); ?>

<?php EndPage(); ?>

