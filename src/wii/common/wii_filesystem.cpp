//Stolen from http://code.google.com/p/dop-mii

#include <cstdio>
#include <cstdarg>
#include <string.h>
#include <malloc.h>
#include <sys/dir.h>
#include <unistd.h>
#include <sdcard/wiisd_io.h>
#include <fat.h>
#include <ogcsys.h>
#include "wii_usbstorage.h"
#include "wii_filesystem.h"

#define CACHE 32
#define SECTORS 64
#define SECTORS_SD 32

namespace IO 
{
	void SD::Unmount()
	{
		fatUnmount("sd:/");
		__io_wiisd.shutdown();
	}

	bool SD::Mount()
	{			
		SD::Unmount();
		return fatMountSimple("sd", &__io_wiisd);
	}

	bool USB::isMounted = false;

	void USB::Unmount()
	{
		fatUnmount("usb:/");
		isMounted = false;
	}

	bool USB::Mount()
	{
		if (isMounted) return true;
		//gprintf("Mounting USB Drive \n");
		USB::Unmount();

		// To Hopefully Wake Up The Drive
		fatMountSimple("usb", &__io_usbstorage);

		bool isInserted = __io_usbstorage.isInserted();
		//gprintf("USB::IsInserted = %d\n", isInserted);
		if (!isInserted) return false;

		// USB Drive may be "sleeeeping". 
		// We need to try Mounting a few times to wake it up
		int retry = 10;		
		
		while (retry)
		{
			isMounted = fatMountSimple("usb", &__io_usbstorage);
			if (isMounted) break;
			sleep(1);
			retry--;
		}
		
		//if (isMounted) gprintf("USB Drive Is Mounted\n");
		return isMounted;		
	}

	void USB::Startup()
	{
		USB::Unmount();
		// To Hopefully Wake Up The Drive				
		isMounted = fatMountSimple("usb", &__io_usbstorage);
	}

	void USB::Shutdown()
	{
		if (!isMounted) return;
		fatUnmount("usb:/");
		isMounted = false;
	}
}
