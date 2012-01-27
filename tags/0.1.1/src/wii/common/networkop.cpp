/****************************************************************************
* FCE Ultra
* Nintendo Wii/Gamecube Port
*
* Tantric December 2008
*
* networkop.cpp
*
* Network and SMB support routines
****************************************************************************/

#include <gccore.h>
#include <network.h>
#include <malloc.h>
#include <ogc/lwp_watchdog.h>
#include <smb.h>
#include <errno.h>

#include "fileop.h"
#include "gettext.h"
#include "wii_util.h"

static bool networkInit = false;
static bool networkShareInit = false;
char wiiIP[16] = { 0 };

static int netHalt = 0;

static lwp_t networkthread = LWP_THREAD_NULL;
static u8 netstack[32768] ATTRIBUTE_ALIGN (32);
static char username[512] = "";
static char password[512] = "";
static char share[512] = "";
static char ipaddress[17] = "";
static char errorMessage[512] = "";

void setSmbUser( const char* name )
{
  Util_strlcpy( username, name, sizeof(username) );
}

const char* getSmbUser()
{
  return username;
}

void setSmbPassword( const char* pass )
{
  Util_strlcpy( password, pass, sizeof(password) );
}

const char* getSmbPassword()
{
  return password;
}

void setSmbShare( const char* s )
{
  Util_strlcpy( share, s, sizeof(share) );
}

const char* getSmbShare()
{
  return share;
}

void setSmbAddress( const char* ip )
{
  Util_strlcpy( ipaddress, ip, sizeof(ipaddress) );
}

const char* getSmbAddress()
{
  return ipaddress;
}

static void setErrorMessage( const char* msg )
{
  Util_strlcpy( errorMessage, msg, sizeof(errorMessage) );
}

void resetSmbErrorMessage()
{
  errorMessage[0] = '\0';
}

const char* getSmbErrorMessage()
{
  return strlen(errorMessage) > 0 ? errorMessage : NULL;
}

static void * netcb (void *arg)
{
  s32 res=-1;
  int retry;
  int wait;
  static bool prevInit = false;

  while(netHalt != 2)
  {
    retry = 5;

    while (retry>0 && (netHalt != 2))
    {			
      if(prevInit) 
      {
        int i;
        net_deinit();
        for(i=0; i < 400 && (netHalt != 2); i++) // 10 seconds to try to reset
        {
          res = net_get_status();
          if(res != -EBUSY) // trying to init net so we can't kill the net
          {
            usleep(2000);
            net_wc24cleanup(); //kill the net 
            prevInit=false; // net_wc24cleanup is called only once
            usleep(20000);
            break;					
          }
          usleep(20000);
        }
      }

      usleep(2000);
      res = net_init_async(NULL, NULL);

      if(res != 0)
      {
        sleep(1);
        retry--;
        continue;
      }

      res = net_get_status();
      wait = 400; // only wait 8 sec
      while (res == -EBUSY && wait > 0  && (netHalt != 2))
      {
        usleep(20000);
        res = net_get_status();
        wait--;
      }

      if(res==0) break;
      retry--;
      usleep(2000);
    }
    if (res == 0)
    {
      struct in_addr hostip;
      hostip.s_addr = net_gethostip();
      if (hostip.s_addr)
      {
        strcpy(wiiIP, inet_ntoa(hostip));
        networkInit = true;	
        prevInit = true;
      }
    }
    if(netHalt != 2) LWP_SuspendThread(networkthread);
  }
  return NULL;
}

/****************************************************************************
* StartNetworkThread
*
* Signals the network thread to resume, or creates a new thread
***************************************************************************/
void StartNetworkThread()
{
  netHalt = 0;

  if(networkthread == LWP_THREAD_NULL)
    LWP_CreateThread(&networkthread, netcb, NULL, netstack, 8192, 40);
  else
    LWP_ResumeThread(networkthread);
}

/****************************************************************************
* StopNetworkThread
*
* Signals the network thread to stop
***************************************************************************/
void StopNetworkThread()
{
  if(networkthread == LWP_THREAD_NULL || !LWP_ThreadIsSuspended(networkthread))
    return;

  netHalt = 2;
  LWP_ResumeThread(networkthread);

  // wait for thread to finish
  LWP_JoinThread(networkthread, NULL);
  networkthread = LWP_THREAD_NULL;
}


bool InitializeNetwork(int retryCount)
{
  StopNetworkThread();

  if(networkInit && net_gethostip() > 0)
    return true;

  networkInit = false;

  retryCount++;
  if( retryCount <= 0 )
  {
    retryCount = 1;
  }

  while(retryCount)
  {
    u64 start = gettime();
    StartNetworkThread();

    while (!LWP_ThreadIsSuspended(networkthread))
    {
      usleep(50 * 1000);

      if(diff_sec(start, gettime()) > 10) // wait for 10 seconds max for net init
        break;
    }

    if(networkInit)
      break;

    setErrorMessage( gettextmsg("Unable to initialize network!") );

    if(networkInit && net_gethostip() > 0)
      return true;

    retryCount--;
  }
  return networkInit;
}

void CloseShare()
{
  if(networkShareInit)
    smbClose("smb");
  networkShareInit = false;
  isMounted[DEVICE_SMB] = false;
}

/****************************************************************************
* Mount SMB Share
****************************************************************************/

bool
ConnectShare (int retryCount)
{
  if(!InitializeNetwork(retryCount))
    return false;

  if(networkShareInit)
    return true;

  retryCount++;
  if( retryCount <= 0 )
  {
    retryCount = 1;
  }

  int chkS = (strlen(share) > 0) ? 0:1;
  int chkI = (strlen(ipaddress) > 0) ? 0:1;

  // check that all parameters have been set
  if(chkS + chkI > 0)
  {
    char msg[50];
    char msg2[100];
    if(chkS + chkI > 1) // more than one thing is wrong
      sprintf(msg, "Check configuration file.");
    else if(chkS)
      sprintf(msg, "Share name is blank.");
    else if(chkI)
      sprintf(msg, "Share IP is blank.");

    sprintf(msg2, "%s - %s", 
      gettextmsg("Invalid network settings"), gettextmsg(msg));

    setErrorMessage( msg2 );

    return false;
  }

  while(retryCount)
  {
    if(smbInit(username, password, share, ipaddress))
      networkShareInit = true;

    if(networkShareInit )
      break;

    retryCount--;
  }

  return networkShareInit;
}