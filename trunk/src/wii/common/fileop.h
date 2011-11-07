/****************************************************************************
 * Snes9x Nintendo Wii/Gamecube Port
 *
 * softdev July 2006
 * crunchy2 May 2007
 * Michniewski 2008
 * Tantric 2008-2010
 *
 * fileop.h
 *
 * File operations
 ****************************************************************************/

#ifndef _FILEOP_H_
#define _FILEOP_H_

#include <gccore.h>
#include <stdio.h>
#include <string.h>
#include <ogcsys.h>
#include <fat.h>
#include <unistd.h>

#define FS_RETRY_COUNT 15

const char pathPrefix[3][8] =
{ "sd:/", "usb:/", "smb:/" };

enum {
  DEVICE_SD,
  DEVICE_USB,
  DEVICE_SMB
};

void InitDeviceThread();
void ResumeDeviceThread();
void HaltDeviceThread();
void MountAllFAT();
void UnmountAllFAT();
bool FindDevice(char * filepath, int * device);
char * StripDevice(char * path);
bool ChangeInterface(int device, int retryCount);
bool ChangeInterface(char * filepath, int retryCount);
void UsbKeepAlive();

extern bool unmountRequired[];
extern bool isMounted[];

#endif