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

#include "main.h"

#include "wii_app.h"
#include "wii_config.h"
#include "wii_gx.h"
#include "wii_input.h"
#include "wii_sdl.h"
#include "wii_snapshot.h"

#include "wii_mednafen.h"
#include "wii_mednafen_main.h"
#include "wii_mednafen_menu.h"
#include "wii_mednafen_snapshot.h"

#include "Emulators.h"
#include "../../../mednafen/src/md5.h"

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"  
#endif

#ifdef MEM2
#include "mem2.h"
#endif

extern "C" void WII_VideoStart();
extern "C" void WII_ChangeSquare(int xscale, int yscale, int xshift, int yshift);

extern bool KillSound(void);

/*
 * Starts the emulator for the specified rom file.
 *
 * romfile  The rom file to run in the emulator
 * savefile The name of the save file to load. If this value is NULL, no save
 *          is explicitly loaded (auto-load may occur). If the value is "", 
 *          no save is loaded and auto-load is ignored (used for reset).
 * reset    Whether we are resetting the current game
 * resume   Whether we are resuming the current game
 */
void wii_start_emulation( char *romfile, const char *savefile, bool reset, bool resume )
{
  // Write out the current config
  wii_write_config();

  // Set auto load/save state
  MDFNI_SetSettingB( "autosave", wii_auto_load_save_state );

  bool succeeded = true;

  Emulator *emulator;

  // Start emulation
  if( !resume && !reset )
  {
#ifdef MEM2
    Mem2ManagerReset(); // Reset the MEM2 manager
#endif

    // Reset the control mappings
    wii_mednafen_set_reset_controls();

    wii_cartridge_hash[0] = '\0'; // Reset the cartridge hash
    wii_cartridge_hash_with_header[0] = '\0';
    wii_snapshot_reset(); // Reset snapshot related state
    wii_mednafen_set_message( NULL ); // Reset the display message

    emuRegistry.setCurrentEmulator( NULL ); // Reset the current emulator
    KillSound(); // Kill Sound?
    succeeded = ( wii_mednafen_load_game( romfile ) != 0 );    

    if( succeeded )
    { 
      strcpy( 
        wii_cartridge_hash_with_header, 
        md5_context::asciistr(MDFNGameInfo->MD5, 0).c_str() );

      if( wii_cartridge_hash[0] == '\0' )
      {
        strcpy( 
          wii_cartridge_hash, 
          md5_context::asciistr(MDFNGameInfo->MD5, 0).c_str() );
      }

      // Set the current emulator
      emulator =
        emuRegistry.setCurrentEmulator( MDFNGameInfo->shortname );
      
      // Look up the cartridge in the database
      DatabaseManager& dbManager = emulator->getDbManager();
      dbManager.loadEntry( wii_cartridge_hash );

      // Give the emulator an opportunity to do something after the load
      emulator->onPostLoad();

#ifdef WII_NETTRACE
      dbEntry* entry = dbManager.getEntry();
      net_print_string( NULL, 0, "System: %s\n", MDFNGameInfo->shortname );
      net_print_string( NULL, 0, "%s, %s, %s\n", 
        entry->name, wii_cartridge_hash_with_header, wii_cartridge_hash );
#endif

#if 0
      // Load the save if applicable
      if( !reset && succeeded &&
          ( savefile != NULL && strlen( savefile ) > 0 ) )
      {
        // Ensure the save is valid
        int sscheck = wii_check_snapshot( savefile );
        if( sscheck < 0 )
        {
          if( sscheck == -2 )            
          {
            wii_set_status_message(
              "The save specified is not valid." );                
          }
          else
          {
            wii_set_status_message(
              "Unable to find the specified save state file." );                
          }

          succeeded = false;
        }
        else
        {
          succeeded = MDFNI_LoadState( savefile, NULL );                    
          if( !succeeded )
          {
            wii_set_status_message(
              "Error loading the specified save state file." );                
          }
        }
      }
#endif
    }
  }
  else if( reset )
  {
    MDFNGameInfo->DoSimpleCommand( MDFN_MSC_POWER );
  }

  if( succeeded )
  {    
    loading_game = FALSE; // Update menu

    // Blank the screen
    wii_gx_push_callback( NULL, FALSE, NULL );    

    // Reset status message count
    wii_status_message_count = 0;

    int retVal = 1;
    if( !resume && !reset )
    {
      // Wait until no buttons are pressed
      wii_wait_until_no_buttons( 2 );

      // Show the controls screen (if applicable)
      retVal = emulator->onShowControlsScreen();
    }

    // This is currently disabled due to the fact that if you exit from
    // the controls screen, you can attempt to "resize", but the screen 
    // has not been displayed which will lead to a dump...
//    if( retVal )
//    {
      // Wait until no buttons are pressed
      wii_wait_until_no_buttons( 2 );

      // Start the emulator loop
      wii_mednafen_emu_loop( resume );            
//    }

    // Pop the callback
    wii_gx_pop_callback();

    // Store the name of the last rom (for resuming later)        
    // Do it in this order in case they passed in the pointer
    // to the last rom variable
    char *last = strdup( romfile );
    if( wii_last_rom != NULL )
    {
      free( wii_last_rom );    
    }

    wii_last_rom = last;

    if( wii_top_menu_exit )
    {
      // Pop to the top
      while( wii_menu_pop() != NULL );
    }
  } 
  else
  {
    // Reset the last rom that was loaded
    if( wii_last_rom != NULL )
    {
      free( wii_last_rom );
      wii_last_rom = NULL;
    }
  }

  // Wait until no buttons are pressed
  wii_wait_until_no_buttons( 2 );
}

/*
 * Resumes emulation of the current game
 */
void wii_resume_emulation()
{
  wii_start_emulation( wii_last_rom, "", false, true );
}

/*
 * Resets the current game
 */
void wii_reset_emulation()
{
  wii_start_emulation( wii_last_rom, "", true, false );
}