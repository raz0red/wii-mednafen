/****************************************************************************
 * FCE Ultra
 * Nintendo Wii/Gamecube Port
 *
 * Tantric December 2008
 *
 * networkop.h
 *
 * Network and SMB support routines
 ****************************************************************************/

#ifndef _NETWORKOP_H_
#define _NETWORKOP_H_

#define NET_RETRY_COUNT 0

void StartNetworkThread();
bool InitializeNetwork(int retryCount);
bool ConnectShare (int retryCount);
void CloseShare();

void setSmbUser( const char* name );
const char* getSmbUser();
void setSmbPassword( const char* pass );
const char* getSmbPassword();
void setSmbShare( const char* s );
const char* getSmbShare();
void setSmbAddress( const char* ip );
const char* getSmbAddress();

void resetSmbErrorMessage();
const char* getSmbErrorMessage();

#endif