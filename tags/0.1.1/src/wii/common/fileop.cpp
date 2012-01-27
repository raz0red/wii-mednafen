/****************************************************************************
* Snes9x Nintendo Wii/Gamecube Port
*
* softdev July 2006
* crunchy2 May 2007
* Michniewski 2008
* Tantric 2008-2010
*
* fileop.cpp
*
* File operations
***************************************************************************/

#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogcsys.h>
#include <dirent.h>
#include <sys/stat.h>
#include <zlib.h>
#include <malloc.h>
#include <sdcard/wiisd_io.h>
#include <sdcard/gcsd.h>
#include <ogc/usbstorage.h>
#include <di/di.h>
#if 0
#include <ogc/dvd.h>
#include <iso9660.h>
#endif

#include "fileop.h"
#include "networkop.h"
#include "sdl.h"
#include "net_print.h"

#define THREAD_SLEEP 100

bool unmountRequired[3] = { false, false, false };
bool isMounted[3] = { false, false, false };

const DISC_INTERFACE* sd = &__io_wiisd;
const DISC_INTERFACE* usb = &__io_usbstorage;

#if 0
// device thread
static lwp_t devicethread = LWP_THREAD_NULL;
static bool deviceHalt = true;

/****************************************************************************
* ResumeDeviceThread
*
* Signals the device thread to start, and resumes the thread.
***************************************************************************/
void
ResumeDeviceThread()
{
  deviceHalt = false;
  LWP_ResumeThread(devicethread);
}

/***************************************************************************
* HaltGui
*
* Signals the device thread to stop.
***************************************************************************/
void
HaltDeviceThread()
{
  deviceHalt = true;

  // wait for thread to finish
  while(!LWP_ThreadIsSuspended(devicethread))
    usleep(THREAD_SLEEP);
}

/****************************************************************************
* devicecallback
*
* This checks our devices for changes (SD/USB/DVD removed)
***************************************************************************/
static int devsleep;

static void *
devicecallback (void *arg)
{
  while (1)
  {
    if(isMounted[DEVICE_SD])
    {
      if(!sd->isInserted()) // check if the device was removed
      {
        unmountRequired[DEVICE_SD] = true;
        isMounted[DEVICE_SD] = false;
      }
    }

    if(isMounted[DEVICE_USB])
    {
      if(!usb->isInserted()) // check if the device was removed
      {
        unmountRequired[DEVICE_USB] = true;
        isMounted[DEVICE_USB] = false;
      }
    }

    devsleep = 1000*1000; // 1 sec

    while(devsleep > 0)
    {
      if(deviceHalt)
        LWP_SuspendThread(devicethread);
      usleep(THREAD_SLEEP);
      devsleep -= THREAD_SLEEP;
    }
    UpdateCheck();
  }
  return NULL;
}

/****************************************************************************
* InitDeviceThread
*
* libOGC provides a nice wrapper for LWP access.
* This function sets up a new local queue and attaches the thread to it.
***************************************************************************/
void
InitDeviceThread()
{
  LWP_CreateThread (&devicethread, devicecallback, NULL, NULL, 0, 40);
}
#endif

/****************************************************************************
* UnmountAllFAT
* Unmounts all FAT devices
***************************************************************************/
void UnmountAllFAT()
{
  fatUnmount("sd:");
  fatUnmount("usb:");
}

/****************************************************************************
* MountFAT
* Checks if the device needs to be (re)mounted
* If so, unmounts the device
* Attempts to mount the device specified
* Sets libfat to use the device by default
***************************************************************************/

static bool MountFAT(int device, int retryCount)
{ 
  bool mounted = false;
  char name[10], name2[10];
  const DISC_INTERFACE* disc = NULL;

  retryCount++;
  if( retryCount <= 0 )
  {
    retryCount = 1;
  }

  switch(device)
  {
    case DEVICE_SD:
      sprintf(name, "sd");
      sprintf(name2, "sd:");
      disc = sd;
      break;
    case DEVICE_USB:
      sprintf(name, "usb");
      sprintf(name2, "usb:");
      disc = usb;
      break;
    default:
      return false; // unknown device
  }

  if(unmountRequired[device])
  {
    unmountRequired[device] = false;
    fatUnmount(name2);
    disc->shutdown();
    isMounted[device] = false;
  }

  while(retryCount)
  {
    if(disc->startup() && fatMountSimple(name, disc))
      mounted = true;

    if(mounted)
      break;

    usleep( 1000 * 1000 ); // 1 second
    retryCount--;
  }

  isMounted[device] = mounted;
  return mounted;
}

void MountAllFAT()
{
  MountFAT(DEVICE_SD, 1);
  MountFAT(DEVICE_USB, 1);
}

bool FindDevice(char * filepath, int * device)
{
  if(!filepath || filepath[0] == 0)
    return false;

  if(strncmp(filepath, "sd:", 3) == 0)
  {
    *device = DEVICE_SD;
    return true;
  }
  else if(strncmp(filepath, "usb", 3) == 0)
  {
    *device = DEVICE_USB;
    return true;
  }
  else if(strncmp(filepath, "smb:", 4) == 0)
  {
    *device = DEVICE_SMB;
    return true;
  }
  return false;
}

char * StripDevice(char * path)
{
  if(path == NULL)
    return NULL;

  char * newpath = strchr(path,'/');

  if(newpath != NULL)
    newpath++;

  return newpath;
}

/****************************************************************************
* ChangeInterface
* Attempts to mount/configure the device specified
***************************************************************************/
bool ChangeInterface(int device, int retryCount )
{
  if(isMounted[device])
    return true;

  bool mounted = false;

  switch(device)
  {
    case DEVICE_SD:
    case DEVICE_USB:
      mounted = MountFAT(device, retryCount);
      break;
    case DEVICE_SMB:
      mounted = ConnectShare(NET_RETRY_COUNT);
      break;
  }

  return mounted;
}

bool ChangeInterface(char * filepath, int retryCount )
{
  int device = -1;

  if(!FindDevice(filepath, &device))
    return false;

  return ChangeInterface(device, retryCount );
}

static u32 lastKeepAlive = 0;
static u32 keepAliveCount = 0;
static FILE* keepAliveFile = NULL;
#define KEEP_ALIVE_FILE "usb:/_keepalive.tmp"
#define KEEP_ALIVE (60 * 1000 * 9)

void UsbKeepAlive()
{
  if( isMounted[DEVICE_USB] )
  {
    u32 time = SDL_GetTicks();
    if( ( time - lastKeepAlive ) > KEEP_ALIVE )
    {
      lastKeepAlive = time;
      keepAliveCount++;
      keepAliveCount %= 100;
      //if( !keepAliveFile )
      //{
        keepAliveFile = fopen( KEEP_ALIVE_FILE, "w" );
      //}
      BOOL success = 0;
      if( keepAliveFile )
      {
        fputc( keepAliveCount, keepAliveFile );
        fflush( keepAliveFile );
        fclose( keepAliveFile );
        success = 1;
      }
#ifdef WII_NETTRACE
      net_print_string( NULL, 0, 
        "usb keepalive:%s, %d\n", KEEP_ALIVE_FILE, success );  
#endif
    }
  }
}