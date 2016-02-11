--------------------------------------------
WiiMednafen v0.2-pre release README
--------------------------------------------

Ported by raz0red

_Wii Mednafen 0.1 Update Video_

<a href='http://www.youtube.com/watch?feature=player_embedded&v=JKMr3YnTHfg' target='_blank'><img src='http://img.youtube.com/vi/JKMr3YnTHfg/0.jpg' width='425' height=344 /></a>

_Wii Mednafen Overview Video_

<a href='http://www.youtube.com/watch?feature=player_embedded&v=qpWFx-QGfb8' target='_blank'><img src='http://img.youtube.com/vi/qpWFx-QGfb8/0.jpg' width='425' height=344 /></a>

--------------------------------------------
Overview
--------------------------------------------

WiiMednafen is a partial port of the Mednafen emulator.
[http://mednafen.sourceforge.net/]

Emulators supported:

 * GameBoy/GameBoy Color
 * GameBoy Advance
 * Game Gear
 * Lynx
 * MegaDrive/Genesis
 * Nintendo Entertainment System (NES)
 * Neo Geo Pocket
 * PC Engine (CD)/TurboGrafx 16 (CD)/SuperGrafx (Fast version)
 * PC-FX
 * Sega Master System
 * Virtual Boy
 * WonderSwan

It should be noted that the Virtual Boy emulation included with this emulator
is much improved over WiirtualBoy. Most games now run at 100% speed without
frame skipping. These improvements are due entirely to Ryphecha (Mednafen)
who optimized the core emulation code in addition to eliminating idle loops
on a per game basis.

--------------------------------------------
Why port Mednafen?
--------------------------------------------

So, a common question might be, "Why release a Mednafen port when a number
of the included emulators already exist for the Wii?". After releasing the
port of the Mednafen Virtual Boy emulator (WiirtualBoy), my intent was to
release a port of the WonderSwan emulator. However, along the way, I decided
that rather than replicate a lot of code between projects, I would release
a port of Mednafen itself. Also, the possibility does exist for some issues
to be resolved with the Mednafen code base. For example, the Lynx port does
resolve some of the graphical glitches that exist in WiiHandy (RoadBlasters,
Cyberball 2072).

--------------------------------------------
Known issues
--------------------------------------------

This is a very early release of this port, and as such there are a number of
known issues. Further, this emulator hasn't had a tremendous amount of testing
so I am fairly certain there are a number of unknown issues as well. Please 
let me know if you experience any via the Google Code project issues list.

 * Save states do not display an associated screenshot.
 * PC-FX emulation is extremely slow.
 * Loading CD images via SMB (Network) will cause noticeable slowdown.
 * Cheats can't be created/edited within the WiiMednafen interface. (They
   must be created manually or preferably via Mednafen itself).
 * Individual cheats can't be enabled/disabled. Currently, all cheats are
   either enabled or disabled globally (via "Advanced" settings).
 * If you have a hard drive that falls asleep, it is *highly* recommended
   that you install the application and its support files on an SD card.
   This ensures that configuration changes and save states will persist
   properly, even if the drive goes to sleep.
 * Many emulator options are not currently exposed, they will be added with
   subsequent releases.
 * Several features of Mednafen proper are not currently supported, they will
   be added with subsequent releases.
 * The front-end is functional but minimal. This is something that may be 
   addressed once I am satisfied with the overall state of the port. 
 * Some Mega Drive/Genesis games may not run at 100% speed (Thunder Force IV).

--------------------------------------------
Installation
--------------------------------------------

To install WiiMednafen, extract the zip file directly to your SD card or
USB device (retain the hierarchical structure exactly).

Game ROMs and CD images can be loaded via SD, USB, and SMB (Network).

*NOTE* The "/wiimednafen" directory (containing saves, BIOS images, etc.) must
reside on the same device as the "/apps/wiimednafen" directory (containing the
application binary). These two directories must not be split across two
different devices. However, game ROMs and CD images can be loaded from any of
the available devices (SD, USB, or SMB (Network)).

 Emulator-specific files:
 ------------------------

 To function properly, several emulators require additional files
 (BIOS images, etc.). These files must be placed in the "/wiimednafen"
 directory.

 * lynxboot.img             : Lynx boot ROM image.
 * pcfx.rom                 : PC-FX BIOS image.
 * syscard3.pce             : PC Engine CD BIOS
 * disksys.rom              : Famicom Disk System BIOS
 * gg.rom (Optional)        : NES Game Genie ROM image
 * gba_bios.bin (Optional)  : GameBoy Advance BIOS

--------------------------------------------
SMB (Network support)
--------------------------------------------

WiiMednafen allows for loading ROM and CD images over the network via SMB.
To enable SMB support, you must edit the "wiimednafen.conf" file located
in the "/wiimednafen" directory and provide values for the following:

 * share_ip    : IP address of the computer to connect to.
 * share_name  : Name of the share on the computer.
 * share_user  : Name of the user to connect as
 * share_pass  : Password for the user

Please note that loading CD images over the network will cause noticeable
slowdown.

--------------------------------------------
CD-ROM Emulation
--------------------------------------------

There has been some discussion about the types of CD-ROM emulation formats
WiiMednafen supports, so I added this section in hopes of clearing up any
misconceptions. There are really two types of CD-ROM emulation formats
supported by WiiMednafen. 

  CUE+BIN
  -------
  
  CUE+BIN is the standard format popularized by CDRWin. All of the tracks are
  contained within a single BIN file. The CUE file indicates where each of the
  tracks are located within that BIN file. This is the preferred method for
  WiiMednafen since a properly dumped disk will always work.
  
  CUE+ISO+Audio(Ogg Vorbis and MusePack)
  --------------------------------------
  
  In this format, the CUE file refers to one or more external files that
  represent the different tracks (versus all being in one BIN file). The data
  tracks are typically represented by ISO files, while the audio tracks 
  are represented by various audio format files. WiiMednefan only supports Ogg
  Vorbis and MusePack audio formats (MP3, WAV, FLAC, etc. are not currently 
  supported). 
  
  Many people have created dumps of PC Engine and PC-FX games in this format
  in an effort to reduce the overall game size. Unfortunately, many of these
  dumps leverage the MP3 audio format that is not support by WiiMednafen 
  (or Mednafen proper) due to patent issues.
  
So, in summary CUE+BIN of a properly dumped game will always work with 
WiiMednafen. If you want to reduce the size of a dump, you must use Ogg Vorbis
or MusePack audio formats. In the future, I may look at adding additional
audio formats (FLAC, WAV, etc.) based on the feedback I receive. However, in
keeping in alignment with Mednafen proper, MP3 support will never be added.
  
--------------------------------------------
Double strike (240p) mode support
--------------------------------------------

Double strike (240p) mode is supported for all emulators except Virtual Boy.
You can enable double strike globally via "Advanced" > "Video Settings" >
"Double strike". The global value can be overridden a per-emulator basis
via "Emulator settings" > "Double strike".

By default, double strike is explicitly disabled for all handheld systems.
If you want to enable double strike for those emulators, you must set their
double strike mode (in "Emulator settings") to either "(default)" (observe
global setting) or "Enabled" (explicitly enable double strike regardless of
global setting).

*NOTE* Double strike for PAL-based games (288p) has not been implemented. If
there is enough interest, I will look into adding support in a future release.

--------------------------------------------
Rewind support
--------------------------------------------

*NOTE* Rewind support should be considered "experimental" for the current 
release. Enabling rewind may cause performance and/or stability issues. If 
you experience odd performance behavior or periodic crashes, you should 
consider disabling rewind.

Due to the amount of memory and processing required, rewind support is
currently limited to the following emulators:

 * GameBoy/GameBoy Color
 * Game Gear
 * Lynx 
 * Nintendo Entertainment System (excludes FDS games)
 * Neo Geo Pocket
 * PC Engine/TurboGrafx 16 (excludes CD games)
 * Sega Master System
 * WonderSwan (excludes large SRAM sizes: Dicing Knight, Judgment Silversword)

 Enabling rewind:
 ----------------
 
 To enable rewind, set the "Advanced" > "Rewind" option to "Enabled". 

 Rewind buttons (auto vs. manual)
 --------------------------------
 
 When rewind is enabled, an additional menu item is displayed,
 "Rewind buttons", that controls how rewind buttons are added to the 
 different controllers supported by WiiMednafen (Wiimote, Wiimote + Nunchuk, 
 Classic, GameCube).
 
 When "auto" mode is selected, each of the supported controllers are examined
 in turn to determine if a "(rewind)" button has been explicitly mapped (via 
 "control settings" for the current game). If a "(rewind)" button has not
 been explicitly mapped for a controller, the "default" rewind button is added
 automatically (see the "default rewind buttons" table below).
 
 When "manual" mode is selected, rewind buttons must be mapped manually for
 each controller via a game's "control settings" (available in the 
 "Game-specific settings" menu). You might be wondering why you would ever
 want to use manual mode versus having the buttons added automatically. One 
 reason is that with the manual mapping mode, a rewind button does not need
 to be added to each of the controllers. For example, you might be playing a 
 game that requires all of the buttons on the GameCube controller so you don't
 have one available for the rewind button. You could set the mode to manual and 
 only map the rewind button to the Wiimote. Therefore, you can play the game 
 using all of the GameCube's buttons, and still have the capability to rewind 
 via the Wiimote.
 
   Default rewind buttons (auto):
   ------------------------------

     Wiimote:

       B  : Rewind

     Wiimote + Nunchuk:

       2  : Rewind

     Classic controller/Pro:

       zL : Rewind
     
     GameCube controller:

       L  : Rewind
 
--------------------------------------------
Menu settings and navigation
--------------------------------------------

When exiting to the emulator menu after loading a ROM or CD image, two new
items will be available, "Game-specific settings" and "Emulator settings".

 Emulator settings:
 ------------------

 Emulator settings contain settings specific to the emulator that is
 associated with the current game. For example, the Virtual Boy emulator
 settings allows for specifying the display mode to use (2D vs. 3D, etc.).

 Game-specific settings:
 -----------------------

 Game-specific settings contain settings that are specific to the current
 game. These settings include the ability to provide customized button 
 mappings, etc.

 Menu Controls:
 --------------

   Wiimote:

     Left/Right  : Scroll (if sideways orientation)
     Up/Down     : Scroll (if upright orientation)
     A/2         : Select
     B/1         : Back
     Home        : Exit to Homebrew Channel
     Power       : Power off

   Classic controller/Pro:

     Up/Down     : Scroll
     A           : Select
     B           : Back
     Home        : Exit to Homebrew Channel

   Nunchuk controller:

     Up/Down     : Scroll
     C           : Select
     Z           : Back

   GameCube controller:

     Up/Down     : Scroll
     A           : Select
     B           : Back
     Z           : Exit to Homebrew Channel

--------------------------------------------
GameBoy/GameBoy Color
--------------------------------------------

Below are the "default" control mappings for the GameBoy/GameBoy Color
emulator. Controls can be overridden on a game-by-game basis via the
"Game-specific settings" menu item.

 Controls:
 --------------

   Wiimote:

     D-pad          : Move
     2              : A
     1              : B
     +              : Start
     -              : Select
     Home           : Display WiiMednafen menu (see above)

   Wiimote + Nunchuk:

     Analog         : Move
     A              : A
     C              : B
     +              : Start
     -              : Select
     Home           : Display WiiMednafen menu (see above)

   Classic controller/Pro:

     D-pad/Analog   : Move
     A              : A
     B              : B
     +              : Start
     -              : Select
     Home           : Display WiiMednafen menu (see above)

   GameCube controller:

     D-pad/Analog   : Move
     A              : A
     B              : B
     Start          : Start
     X/Y            : Select
     Home           : Display WiiMednafen menu (see above)

--------------------------------------------
GameBoy Advance
--------------------------------------------

The GameBoy Advance emulator supports the use of a real BIOS (versus the 
built-in BIOS). To use a real BIOS, name the BIOS file "gba_bios.bin" and
place it in the "/wiimednafen" directory. Then, change the "External BIOS"
setting in the "Emulator settings" to "Enabled" (to get to this menu, you must
load a GBA game and exit back to the menu).

Below are the "default" control mappings for the GameBoy Advance emulator.
Controls can be overridden on a game-by-game basis via the
"Game-specific settings" menu item.

 Controls:
 --------------

   Wiimote:

     D-pad          : Move
     2              : A
     1              : B
     B              : L
     A              : R
     +              : Start
     -              : Select
     Home           : Display WiiMednafen menu (see above)

   Wiimote + Nunchuk:

     Analog         : Move
     A              : A
     C              : B
     Z              : L
     B              : R
     +              : Start
     -              : Select
     Home           : Display WiiMednafen menu (see above)

   Classic controller/Pro:

     D-pad/Analog   : Move
     A              : A
     B              : B
     L/zL           : L
     R/zR           : R
     +              : Start
     -              : Select
     Home           : Display WiiMednafen menu (see above)

   GameCube controller:

     D-pad/Analog   : Move
     A              : A
     B              : B
     L              : L
     R              : R
     Start          : Start
     X/Y            : Select
     Home           : Display WiiMednafen menu (see above)

--------------------------------------------
Game Gear
--------------------------------------------

Below are the "default" control mappings for the Game Gear emulator.
Controls can be overridden on a game-by-game basis via the
"Game-specific settings" menu item.

 Controls:
 --------------

   Wiimote:

     D-pad          : Move
     2              : Button 2
     1              : Button 1
     +              : Start
     Home           : Display WiiMednafen menu (see above)

   Wiimote + Nunchuk:

     Analog         : Move
     A              : Button 2
     C              : Button 1
     +              : Start
     Home           : Display WiiMednafen menu (see above)

   Classic controller/Pro:

     D-pad/Analog   : Move
     A              : Button 2
     B              : Button 1
     +              : Start
     Home           : Display WiiMednafen menu (see above)

   GameCube controller:

     D-pad/Analog   : Move
     A              : Button 2
     B              : Button 1
     Start          : Start
     Home           : Display WiiMednafen menu (see above)

--------------------------------------------
Lynx
--------------------------------------------

Lynx emulation requires that the Lynx boot ROM image, "lynxboot.img" exists
in the "/wiimednafen" directory.

Below are the "default" control mappings for the Lynx emulator. Controls can
be overridden on a game-by-game basis via the "Game-specific settings"
menu item.

 Controls:
 --------------

   Wiimote:

     D-pad          : Move
     2              : A
     1              : B
     A              : Pause
     +              : Option 1
     -              : Option 2
     Home           : Display WiiMednafen menu (see above)

   Wiimote + Nunchuk:

     Analog         : Move
     A              : A
     C              : B
     1/2            : Pause
     +              : Option 1
     -              : Option 2
     Home           : Display WiiMednafen menu (see above)

   Classic controller/Pro:

     D-pad/Analog   : Move
     A              : A
     B              : B
     L/zL/R/zR      : Pause
     +              : Option 1
     -              : Option 2
     Home           : Display WiiMednafen menu (see above)

   GameCube controller:

     D-pad/Analog   : Move
     A              : A
     B              : B
     Start          : Pause
     X              : Option 1
     Y              : Option 2
     Home           : Display WiiMednafen menu (see above)

--------------------------------------------
MegaDrive/Genesis
--------------------------------------------

The MegaDrive emulator currently supports up to 2 3-button or 6-button
controllers. Whether to use 3 or 6 button controllers can be specified via the
"Game-specific settings" menu.

Below are the "default" control mappings for the MegaDrive emulator. Controls
can be overridden on a game-by-game basis via the "Game-specific settings"
menu item.

 Controls:
 --------------

   Wiimote:

     D-pad          : Move
     A              : A
     1              : B
     2              : C
     +              : Start
     -              : Mode
     Home           : Display WiiMednafen menu (see above)

   Wiimote + Nunchuk:

     Analog         : Move
     A              : A
     B              : B
     Z              : C
     +              : Start
     -              : Mode
     Home           : Display WiiMednafen menu (see above)

   Classic controller/Pro:

     D-pad/Analog   : Move
     Y              : A
     B              : B
     A              : C
     L/zL           : X (6-button mode)
     X              : Y (6-button mode)
     R/zR           : Z (6-button mode)
     +              : Start
     -              : Mode
     Home           : Display WiiMednafen menu (see above)

   GameCube controller:

     D-pad/Analog   : Move
     B              : A
     A              : B
     X              : C
     L              : X (6-button mode)
     Y              : Y (6-button mode)
     R              : Z (6-button mode)
     Start          : Start
     Home           : Display WiiMednafen menu (see above)
     
--------------------------------------------
Nintendo Entertainment System (NES)
--------------------------------------------

The NES emulator currently supports up to 4 standard controllers, Zapper 
support (via Wiimote IR), Arkanoid controller support (via Wiimote IR, Twist,
and Tilt), Space Shadow gun support (via Wiimote IR), Oeka Kids support 
(via Wiimote IR), and up to 2 Hypershot controllers (Hyper Sports).

To play Famicom Disk System (FDS) games, the disk system BIOS, "disksys.rom"
must exist in the "/wiimednafen" directory.

Below are the "default" control mappings for the NES emulator. Controls can
be overridden on a game-by-game basis via the "Game-specific settings" menu
item.

 Gamepad:
 --------------

   Wiimote:

     D-pad          : Move
     2              : A
     1              : B
     +              : Start
     -              : Select
     A              : *Special (See notes below)
     Home           : Display WiiMednafen menu (see above)

   Wiimote + Nunchuk:

     Analog         : Move
     A              : A
     C              : B
     +              : Start
     -              : Select
     1/2            : *Special (See notes below)
     Home           : Display WiiMednafen menu (see above)

   Classic controller/Pro:

     D-pad/Analog   : Move
     A              : A
     B              : B
     +              : Start
     -              : Select
     L/R/zL/zR      : *Special (See notes below)
     Home           : Display WiiMednafen menu (see above)

   GameCube controller:

     D-pad/Analog   : Move
     A              : A
     B              : B
     Start          : Start
     X/Y            : Select
     R/L            : *Special (See notes below)
     Home           : Display WiiMednafen menu (see above)

 Zapper:
 --------------

   Wiimote:

     IR             : Aim (Point the Wiimote at the screen)
     B              : Zapper Trigger
     2              : A
     1              : B
     +              : Start
     -              : Select
     A              : *Special (See notes below)
     Home           : Display WiiMednafen menu (see above)

 Arkanoid:
 --------------
 
   Wiimote:

     IR             : Move (IR Wiimote mode)
                      In this mode, you point the Wiimote at the screen and 
                      move it left and right
     Tilt           : Move (Tilt Wiimote mode)
                      In this mode, you hold the Wiimote horizontally and 
                      tilt it up and down (like a seesaw)
     Twist          : Move (Twist Wiimote mode)
                      In this mode, you hold the Wiimote horizontally and 
                      twist it forward and backward (like the Wiiware game
                      Bit Trip Beat)     
     2/B            : Arkanoid Button
     A              : A
     1              : B
     +              : Start
     -              : Select     
     Home           : Display WiiMednafen menu (see above)

 Space Shadow Gun:
 -----------------

   Wiimote:

     D-pad          : Move
     IR             : Aim (Point the Wiimote at the screen)
     A              : Grenade
     B              : Fire
     2              : A
     1              : B
     +              : Start (Pause)
     -              : Select
     Home           : Display WiiMednafen menu (see above)

 Hypershot (Hyper Sports):
 -------------------------

   Wiimote:

     D-pad          : Jump
     2/1            : Run
     A              : Jump
     +              : Start
     -              : Select     
     Home           : Display WiiMednafen menu (see above)
     
   Wiimote + Nunchuk:

     Analog         : Jump
     A/B            : Run 
     C/Z            : Jump
     +              : Start
     -              : Select     
     1/2            : *Special (See notes below)
     Home           : Display WiiMednafen menu (see above)

   Classic controller/Pro:

     D-pad/Analog   : Jump
     A/B            : Run
     X/Y            : Jump
     +              : Start
     -              : Select
     L/R/zL/zR      : *Special (See notes below)
     Home           : Display WiiMednafen menu (see above)

   GameCube controller:

     D-pad/Analog   : Jump
     A/B            : Run
     X              : Jump
     Start          : Start     
     Y              : Select
     R/L            : *Special (See notes below)
     Home           : Display WiiMednafen menu (see above)

 Oeka Kids Tablet:
 -----------------

   Wiimote:

     IR (Point)     : Move Cursor (Point the Wiimote at the screen)
     B              : Oeka Kids Button     
     2              : A
     1              : B
     +              : Start
     -              : Select
     Home           : Display WiiMednafen menu (see above)

 *Special is used for "Vs. Unisystem" and "Famicom Disk System" (FDS) games.
 For Vs. Unisystem games this button simulates inserting a coin. For FDS
 games, this button flips the disk (necessary for games that utilize both
 disk sides).

 PAL mode:
 --------------

 Currently, the only way to have a game play in PAL mode is via the file name
 of the ROM. To enable PAL mode for a ROM, ensure the file name contains
 "(P)". In the future I will be adding the ability to force PAL mode via
 "Game-specific settings".

 Game Genie:
 --------------

 Game Genie emulation requires that the Game Genie ROM image, "gg.rom"
 exists in the "/wiimednafen" directory. Game Genie emulation can be
 enabled/disabled via the "Emulator settings" menu.

--------------------------------------------
Neo Geo Pocket
--------------------------------------------

Below are the "default" control mappings for the Neo Geo pocket emulator.
Controls can be overridden on a game-by-game basis via the "Game-specific
settings" menu item.

 Controls:
 --------------

   Wiimote:

     D-pad          : Move
     2              : A
     1              : B
     +              : Option
     Home           : Display WiiMednafen menu (see above)

   Wiimote + Nunchuk:

     Analog         : Move
     A              : A
     C              : B
     +              : Option
     Home           : Display WiiMednafen menu (see above)

   Classic controller/Pro:

     D-pad/Analog   : Move
     A              : A
     B              : B
     +              : Option
     Home           : Display WiiMednafen menu (see above)

   GameCube controller:

     D-pad/Analog   : Move
     A              : A
     B              : B
     Start          : Option
     Home           : Display WiiMednafen menu (see above)

-----------------------------------------------------------
PC Engine (CD)/TurboGrafx 16 (CD)/SuperGrafx (Fast version)
-----------------------------------------------------------

PC Engine CD emulation requires that the PC Engine CD BIOS file,
"syscard3.pce" exists in the "/wiimednafen" directory.

The PC Engine emulator currently supports up to 4 2-button or 6-button
controllers. Whether to use 2 or 6 button controllers can be specified via the
"Game-specific settings" menu.

*NOTE* Using a 6-button controller on a game that has not been designed to 
work with it will exhibit strange behaviors. Only use 6-button controllers on
games that were designed to work with them.

Below are the "default" control mappings for the PC Engine emulator. Controls
can be overridden on a game-by-game basis via the "Game-specific settings"
menu item.

 Controls:
 --------------

   Wiimote:

     D-pad          : Move
     2              : I
     1              : II
     +              : Run
     -              : Select
     Home           : Display WiiMednafen menu (see above)

   Wiimote + Nunchuk:

     Analog         : Move
     A              : I
     C              : II
     +              : Run
     -              : Select
     Home           : Display WiiMednafen menu (see above)

   Classic controller/Pro:

     D-pad/Analog   : Move
     A              : I
     B              : II
     Y              : III (6-button mode)
     L/zL           : IV  (6-button mode)
     X              : V   (6-button mode)
     R/zR           : VI  (6-button mode)
     +              : Run
     -              : Select
     Home           : Display WiiMednafen menu (see above)

   GameCube controller:

     D-pad/Analog   : Move
     A              : I
     B              : II
     Start          : Run
     X/Y            : Select
     Home           : Display WiiMednafen menu (see above)

 SuperGrafx:
 --------------

 SuperGrafx games should be automatically detected and enabled by the
 emulator. However, if you believe that a SuperGrafx game is not being
 detected appropriately (you see a blank screen, etc.) you can force it by
 ensuring that the ROM file name ends in ".sgx".

--------------------------------------------
PC-FX
--------------------------------------------

*NOTE* PC-FX emulation is currently quite slow.

PC-FX emulation requires that the PC-FX BIOS image file, "pcfx.rom" exists
in the "/wiimednafen" directory.

The emulator currently supports up to 4 standard controllers. Control mappings
can be overridden on a game-by-game basis via the "Game-specific settings"
menu item.

 Controls:
 --------------

   Wiimote:

     D-pad          : Move
     2              : I
     1              : II
     +              : Run
     -              : Select
     Home           : Display WiiMednafen menu (see above)

   Wiimote + Nunchuk:

     Analog         : Move
     A              : I
     C              : II
     +              : Run
     -              : Select
     Home           : Display WiiMednafen menu (see above)

   Classic controller/Pro:

     D-pad/Analog   : Move
     A              : I
     B              : II
     Y              : III
     L/zL           : IV
     X              : V
     R/zR           : VI
     +              : Run
     -              : Select
     Home           : Display WiiMednafen menu (see above)

   GameCube controller:

     D-pad/Analog   : Move
     A              : I
     B              : II
     Start          : Run
     X/Y            : Select
     Home           : Display WiiMednafen menu (see above)

--------------------------------------------
Sega Master System
--------------------------------------------

The Master System emulator currently supports up to 2 standard controllers.
Control mappings can be overridden on a game-by-game basis via the
"Game-specific settings" menu item.

 Controls:
 --------------

   Wiimote:

     D-pad          : Move
     2              : Fire 2
     1              : Fire 1
     +              : Pause
     Home           : Display WiiMednafen menu (see above)

   Wiimote + Nunchuk:

     Analog         : Move
     A              : Fire 2
     C              : Fire 1
     +              : Pause
     Home           : Display WiiMednafen menu (see above)

   Classic controller/Pro:

     D-pad/Analog   : Move
     A              : Fire 2
     B              : Fire 1
     +              : Pause
     Home           : Display WiiMednafen menu (see above)

   GameCube controller:

     D-pad/Analog   : Move
     A              : Fire 2
     B              : Fire 1
     Start          : Pause
     Home           : Display WiiMednafen menu (see above)

--------------------------------------------
Virtual Boy
--------------------------------------------

The Virtual Boy emulation included with this emulator is much improved over
WiirtualBoy. Most games now run at 100% speed without frame skipping. These
improvements are due entirely to Ryphecha (Mednafen) who optimized the core
emulation code in addition to eliminating idle loops on a per game basis.

If you do experience any issues, you may want to disable the idle loop
"ROM patching". This can be configured globally via "Emulator settings", or
on a game-by-game basis via "Game-specific settings".

Below are the "default" control mappings for the Virtual Boy emulator.
Controls can be overridden on a game-by-game basis via the "Game-specific
settings" menu item.

 Controls:
 --------------

   Wiimote:

     D-pad          : Left control pad
     2              : A
     1              : B
     A              : R
     B              : L
     +              : Start
     -              : Select
     Home           : Display WiiMednafen menu (see above)

   Wiimote + Nunchuk:

     Analog         : Left control pad
     D-pad          : Right control pad
     A              : A
     C              : B
     B              : R
     Z              : L
     +              : Start
     -              : Select
     Home           : Display WiiMednafen menu (see above)

   Classic controller/Pro:

     D-pad          : Left control pad
     Left analog    : Left control pad
     Right analog   : Right control pad
     A              : A
     B              : B
     R/zR           : R
     L/zL           : L
     +              : Start
     -              : Select
     Home           : Display WiiMednafen menu (see above)

   GameCube controller:

     D-pad          : Left control pad
     Left analog    : Left control pad
     Right analog   : Right control pad
     A              : A
     B              : B
     R              : R
     L              : L
     Start          : Start
     X/Y            : Select
     Home           : Display WiiMednafen menu (see above)

--------------------------------------------
WonderSwan
--------------------------------------------

Below are the "default" control mappings for the WonderSwan emulator.
Controls can be overridden on a game-by-game basis via the "Game-specific
settings" menu item.

 Controls (Normal):
 ------------------

   Wiimote:

     D-pad          : Move
     2              : A
     1              : B
     B              : Y1
     -              : Y3
     A              : Y4
     +              : Start
     Home           : Display WiiMednafen menu (see above)

   Wiimote + Nunchuk:

     Analog         : Move
     A              : A
     C              : B
     Z              : Y1
     1              : Y2
     2/-            : Y3
     B              : Y4
     +              : Start
     Home           : Display WiiMednafen menu (see above)

   Classic controller/Pro:

     D-pad/Analog   : Move
     A              : A
     B              : B
     R/zR           : Y1
     X              : Y2
     Y              : Y3
     L/zL           : Y4
     +              : Start
     Home           : Display WiiMednafen menu (see above)

   GameCube controller:

     D-pad/Analog   : Move
     A              : A
     B              : B
     R              : Y1
     X              : Y2
     Y              : Y3
     L              : Y4
     Start          : Start
     Home           : Display WiiMednafen menu (see above)

 Controls (Rotated):
 -------------------

   Wiimote:

     D-pad          : Move
     -              : B
     B              : X1
     2              : X2
     1              : X3
     A              : X4
     +              : Start
     Home           : Display WiiMednafen menu (see above)

   Wiimote + Nunchuk:

     Analog         : Move
     2              : A
     1/-            : B
     Z              : X1
     A              : X2
     C              : X3
     B              : X4
     +              : Start
     Home           : Display WiiMednafen menu (see above)

   Classic controller/Pro:

     D-pad/Analog   : Move
     R/zR           : A
     L/zL           : B
     Y              : X1
     X              : X2
     A              : X3
     B              : X4
     +              : Start
     Home           : Display WiiMednafen menu (see above)

   GameCube controller:

     D-pad/Analog   : Move
     R              : A
     L              : B
     Y              : X1
     X              : X2
     A              : X3
     B              : X4
     Start          : Start
     Home           : Display WiiMednafen menu (see above)
         
--------------------------------------------
Language support
--------------------------------------------

WiiMednafen supports the ability to switch between different languages via 
the "Advanced" menu's "Language" option. The language translations files must
be located in the "/wiimednafen/lang" directory with each file having a
".lang" extension.

A sample "language" file has been provided as a template, "Hacker.lang". If
you create a translation, please contact me and I will include it in the next
release. Thanks!

--------------------------------------------
WiiMednafen crashes, code dumps, etc.
--------------------------------------------

If you are having issues with WiiMednafen, please let me know about it via one
of the following locations:

[https://github.com/raz0red/wii-mednafen/issues]
[http://www.wiibrew.org/wiki/Talk:WiiMednafen]

--------------------------------------------
Credits / Special Thanks
--------------------------------------------

* Ryphecha        : Providing an incredible multi-emulator, optimizing the
                    Virtual Boy emulation, and putting up with my questions.
* mastershoes     : WiiMednafen channel
* Hielkenator     : Testing, Dutch translation
* cloudskipa      : Testing
* Thiago Alves    : Testing
* Davide Arielli  : Testing
* Arikado         : Helping with the initial Virtual Boy (WiirtualBoy) port.
* Eke-eke         : Double strike help and color trap filter implementation.
* Tantric         : Menu example code and SDL enhancements

--------------------------------------------
Change Log
--------------------------------------------

03/16/12 (0.2-pre)
-------------------
 - NES controls support
   - Added Arkanoid controller support (via Wiimote IR, Twist, and Tilt)
   - Added Space Shadow gun support (via Wiimote IR)
   - Added Zapper support (via Wiimote IR)
   - Added Oeka Kids support (via Wiimote IR)
   - Added Hypershot controller support (Hyper Sports)
 - CD ROM cue sheets 
   - Added support for UTF-8 files.
   - Added support for high-ascii characters.
 - Inherited settings now display the inherited value and where they are 
     being inherited from.
 - Added ability to specify language for WonderSwan via emulator settings.
     (see Mednafen 0.9.19 merge notes below)
 - Merged changes from 0.9.19 (Mednafen proper)
   - LYNX emulation
     - Fixed EVERON detection emulation, and emulated a hardware bug
         regarding SCB data reading, the latter of which fixes a graphical
         glitch in "Joust".
   - NES emulation
     - Improved iNES mapper 69 expansion sound emulation accuracy (volume
         interpretation and channel frequencies were off). (Gimmick! sound)
     - Fixed a bug in the MMC5 split-screen emulation that caused a garbage
         scanline in the intro sequence of "Uchuu Keibitai SDF".      
     - Fixed MMC5 32K PRG switching mode.
   - WonderSwan emulation
     - Fixed a reset initialization bug (due to a design flaw) that was 
         partially corrupting internal and external EEPROM.
     - Implemented experimental setting "wswan.language"; it's only known to
         affect "Digimon Tamers: Battle Spirit".

01/25/12 (0.1.1)
-------------------
 - Minor update from 0.1
   - Double strike fixes for PAL display modes
     - PAL50: fixed minor display corruption issue
     - PAL60: was completely broken, now works correctly
   - Space Pinball Prototype (Virtual Boy)
     - Added idle loop patch, it now runs at full speed w/o frame skipping
     - Added proper control mappings

01/13/12 (0.1)
-------------------
 - Added support for MegaDrive/Genesis.
 - Added rewind support for the following emulators:
   - GameBoy/GameBoy Color
   - Game Gear
   - Lynx 
   - Nintendo Entertainment System (excludes FDS games)
   - Neo Geo Pocket
   - PC Engine/TurboGrafx 16 (excludes CD games)
   - Sega Master System
   - WonderSwan (excludes large SRAM sizes: Dicing Knight, 
       Judgment Silversword)
 - VI + GX scaler (eliminates majority of scaling artifacts). 
 - Default screen sizes per emulator (1x, 2x, Full, etc.).
 - Full widescreen support
 - 16:9 correction option (displays 4:3 on widescreen displays).
 - Double strike (240p) mode support (can be enabled globally and 
     per-emulator). By default, is explicitly disabled for handheld emulators.
 - Color trap filter (Eke-eke implementation) 
 - Master system FM sound support (when region set to Domestic/Japan)
 - Virtual Boy SRAM bug fix (was not being persisted).
 - Fixed bug where screen sizing controls were not being properly rotated
     (games with vertical orientation).
 - Ability to map "rotate screen" button for WonderSwan (Klonoa, etc.).
 - WonderSwan control mappings now supports "profile" toggle. Allows for 
     saving mappings for both orientations for a single game (Klonoa, etc.).
 - Updated Game Gear to display with proper aspect ratio.
 - Ability to set Volume globally and per-emulator.   
 - Additional SMS/GG compatibility
 - Fixed bug that prevented certain Ogg Vorbis files (multiples of 1024, go
     figure) from loading as part of a CD image (CUE sheet).
 - Fixed bug that prevented some FDS games (Metroid) from properly flipping
     the disk.
 
10/10/11 (0.0)
-------------------
 - Initial release
