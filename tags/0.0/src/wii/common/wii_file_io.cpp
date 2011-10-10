/*
Copyright (C) 2010
raz0red (www.twitchasylum.com)

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
#include <gccore.h>

#include <fat.h>
#include <sdcard/wiisd_io.h>
#include <ogc/usbstorage.h>
#include <sdl.h>

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"  
#endif

#include "wii_app.h"

// Is the file system mounted?
static BOOL mounted = FALSE;

/*
 * Unmounts the file system
 */
void wii_unmount()
{
  if( mounted )
  {
    if( wii_is_usb )
    {
      fatUnmount( "usb:/" );
      __io_usbstorage.shutdown(); 
    }
    else
    {
      fatUnmount( "sd:/" );
      __io_wiisd.shutdown();
    }

    mounted = FALSE;
  }
}

/*
 * Mounts the file system
 *
 * return    Whether we mounted the file system successfully
 */
BOOL wii_mount()
{
  if( !mounted )
  {
    if( wii_is_usb )
    {
      if( !__io_usbstorage.startup() || 
          !fatMountSimple( "usb", &__io_usbstorage ) )
      {
        return FALSE;
      }
    }
    else
    {
      if( !__io_wiisd.startup() || 
          !fatMountSimple( "sd", &__io_wiisd ) )
      {
        return FALSE;
      }
    }

    chdir( wii_get_app_path() );

    mounted = TRUE;
  }

  return TRUE;
}

static u32 lastKeepAlive = 0;
static u32 keepAliveCount = 0;
static char keepAliveFile[WII_MAX_PATH] = "";
#define KEEP_ALIVE (60 * 1000 * 5)

void wii_usb_keepalive()
{
#if 0
  if( wii_is_usb )
  {
    if( keepAliveFile[0] == '\0' )
    {
      wii_get_app_relative( "usb.tmp", keepAliveFile );
    }

    u32 time = SDL_GetTicks();
    if( ( time - lastKeepAlive ) > KEEP_ALIVE )
    {
      lastKeepAlive = time;
      keepAliveCount++;
      keepAliveCount %= 100;
      FILE* f = fopen( keepAliveFile, "w" );
      BOOL success = 0;
      if( f )
      {
        fputc( keepAliveCount, f );
        fclose( f );
        success = 1;
      }
#ifdef WII_NETTRACE
      net_print_string( NULL, 0, 
        "usb keepalive:%s, %d\n", keepAliveFile, success );  
#endif
    }
  }
#endif
}
