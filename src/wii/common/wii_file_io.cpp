/*
Copyright (C) 2011
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

#include "wii_app.h"
#include "wii_file_io.h"
#include "wii_filesystem.h"

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"  
#endif

// Is the file system mounted?
static bool mounted = FALSE;
IO::SD sd;
IO::USB usb;

/*
* Unmounts the file system
*/
void wii_unmount()
{
  if( mounted )
  {
    if( wii_is_usb )
    {
      usb.Unmount();
    }
    else
    {
      sd.Unmount();
    }

    mounted = FALSE;
  }
}

/*
* Mounts the file system
*
* return    Whether we mounted the file system successfully
*/
bool wii_mount()
{
  bool ret = false;
  if( !mounted )
  {
    if( wii_is_usb )
    {
      usb.Startup();
      mounted = usb.Mount();
    }
    else
    {
      mounted = sd.Mount();
    }
  }

  return mounted;
}

/*
* Remounts the file system
*/
void wii_remount()
{
  wii_unmount();
  wii_mount();    
}
