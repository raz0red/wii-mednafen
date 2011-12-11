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

#include <stdio.h>

#include "wii_util.h"
#include "wii_mednafen.h"

#include "Emulators.h"
#include "networkop.h"

/*
* Handles reading a particular configuration value
*
* name   The name of the config value
* value  The config value
*/
extern "C" void wii_config_handle_read_value( char *name, char* value )
{
  if( strcmp( name, "debug" ) == 0 )
  {
    wii_debug = Util_sscandec( value );				
  }
  else if( strcmp( name, "top_menu_exit" ) == 0 )
  {
    wii_top_menu_exit = Util_sscandec( value );				
  }
  else if( strcmp( name, "sel_offset" ) == 0 )
  {
    wii_menu_sel_offset = Util_sscandec( value );
  }
  else if( strcmp( name, "sel_color" ) == 0 )
  {
    Util_hextorgba( value, &wii_menu_sel_color );
  }
  else if( strcmp( name, "mote_menu_vertical" ) == 0 )
  {
    wii_mote_menu_vertical = Util_sscandec( value );				
  }
  else if( strcmp( name, "language" ) == 0 )
  {
    Util_strlcpy( wii_language, value, sizeof(wii_language) );
  }
  else if( strcmp( name, "video_filter" ) == 0 )
  {
    wii_filter = Util_sscandec( value );
  }
  else if( strcmp( name, "cheats" ) == 0 )
  {
    wii_cheats = Util_sscandec( value );
  }
  else if( strcmp( name, "double_strike" ) == 0 )
  {
    wii_double_strike_mode = Util_sscandec( value );
  }
  else if( strcmp( name, "full_widescreen" ) == 0 )
  {
    wii_full_widescreen = Util_sscandec( value );
  }
  else if( strcmp( name, "roms_dir" ) == 0 )
  {
    wii_set_roms_dir( value );
  }
  else if( strcmp( name, "share_ip" ) == 0 )
  {
    setSmbAddress( value );
  }
  else if( strcmp( name, "share_name" ) == 0 )
  {
    setSmbShare( value );
  }
  else if( strcmp( name, "share_user" ) == 0 )
  {
    setSmbUser( value );
  }
  else if( strcmp( name, "share_pass" ) == 0 )
  {
    setSmbPassword( value );
  }
  else if( strcmp( name, "usb_keepalive" ) == 0 )
  {
    wii_usb_keepalive = Util_sscandec( value );
  }
  else if( strcmp( name, "trap_filter" ) == 0 )
  {
    wii_trap_filter = Util_sscandec( value );
  }
  else if( strcmp( name, "vi_gx_scaler" ) == 0 )
  {
    wii_gx_vi_scaler = Util_sscandec( value );
  }
  else if( strcmp( name, "16_9_correction" ) == 0 )
  {
    wii_16_9_correction = Util_sscandec( value );
  }  
  else if( strcmp( name, "volume" ) == 0 )
  {
    wii_volume = Util_sscandec( value );
  }  
  else if( strcmp( name, "rewind" ) == 0 )
  {
    wii_rewind = Util_sscandec( value );
  }  
  else if( strcmp( name, "rewind_add_buttons" ) == 0 )
  {
    wii_rewind_add_buttons = Util_sscandec( value );
  }  

  else
  {
    // Read configuration value for emulators
    emuRegistry.readConfigValue( name, value );
  }
}

/*
* Handles the writing of the configuration file
*
* fp   The file pointer
*/
extern "C" void wii_config_handle_write_config( FILE *fp )
{
  fprintf( fp, "share_ip=%s\n", getSmbAddress() );
  fprintf( fp, "share_name=%s\n", getSmbShare() );
  fprintf( fp, "share_user=%s\n", getSmbUser() );    
  fprintf( fp, "share_pass=%s\n", getSmbPassword() );  
  fprintf( fp, "debug=%d\n", wii_debug );
  fprintf( fp, "top_menu_exit=%d\n", wii_top_menu_exit );
  fprintf( fp, "sel_offset=%d\n", wii_menu_sel_offset );    
  fprintf( fp, "mote_menu_vertical=%d\n", wii_mote_menu_vertical );  
  fprintf( fp, "video_filter=%d\n", wii_filter );
  fprintf( fp, "cheats=%d\n", wii_cheats );
  fprintf( fp, "language=%s\n", wii_language );
  fprintf( fp, "roms_dir=%s\n", wii_get_roms_dir() );
  fprintf( fp, "double_strike=%d\n", wii_double_strike_mode );
  fprintf( fp, "full_widescreen=%d\n", wii_full_widescreen );
  fprintf( fp, "usb_keepalive=%d\n", wii_usb_keepalive );
  fprintf( fp, "trap_filter=%d\n", wii_trap_filter );
  fprintf( fp, "vi_gx_scaler=%d\n", wii_gx_vi_scaler );
  fprintf( fp, "16_9_correction=%d\n", wii_16_9_correction );
  fprintf( fp, "volume=%d\n", wii_volume );  
  fprintf( fp, "rewind=%d\n", wii_rewind );  
  fprintf( fp, "rewind_add_buttons=%d\n", wii_rewind_add_buttons );  

  // Write configuration settings for emulators
  emuRegistry.writeConfig( fp );
}