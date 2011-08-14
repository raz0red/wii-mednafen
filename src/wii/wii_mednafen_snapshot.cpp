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
#include "../../mednafen/src/general.h"

#include <stdio.h>

#include "wii_app.h"
#include "wii_util.h"

#include "wii_mednafen.h"
#include "wii_mednafen_emulation.h"
#include "wii_mednafen_main.h"

// Mednafen externs
extern volatile MDFN_Surface *VTReady;
extern volatile MDFN_Rect *VTLWReady;
extern volatile MDFN_Rect *VTDRReady;

static StateStatusStruct* stateStatus = NULL;

void wii_snapshot_reset()
{
  if( stateStatus != NULL ) 
  {
    StateStatusStruct *ss = stateStatus;
    stateStatus = NULL;
    if( ss->gfx != NULL )
    {
      free( ss->gfx );
    }
    free( ss );    
  }
}

int wii_snapshot_current( BOOL* isLatest )
{
  *isLatest = FALSE;
  if( stateStatus == NULL )
  {
    stateStatus = MDFNI_SelectState( 0 );
  }

  if( stateStatus != NULL )
  {
    int curr = stateStatus->current;
    *isLatest = (       
      ( curr == stateStatus->recently_saved ) &&
      stateStatus->status[stateStatus->current] );
    return curr;
  }
  else
  {
    return 0;
  }
}

BOOL wii_snapshot_current_exists()
{
  return stateStatus != NULL &&
    stateStatus->status[stateStatus->current];
}

static void refresh()
{
  if( stateStatus != NULL )
  {
    int index = stateStatus->current;
    wii_snapshot_reset();
    stateStatus = MDFNI_SelectState( index );
  }
}

int wii_snapshot_next()
{
  int nextState = stateStatus != NULL? 
    ( stateStatus->current + 1 ) : 0;

  if( nextState == 10 )
  {
    nextState = 0;
  }
 
  wii_snapshot_reset();
  stateStatus = MDFNI_SelectState( nextState );
}

extern "C" void wii_snapshot_handle_get_name( 
  const char *romfile, char *buffer )
{
  Util_strlcpy( 
    buffer, 
    MDFN_MakeFName(MDFNMKF_STATE,stateStatus->current,NULL).c_str(),
    WII_MAX_PATH );
}

extern "C" BOOL wii_snapshot_handle_save( char* filename )
{  
  BOOL success = 
    MDFNI_SaveState(
      filename, NULL, 
      (const MDFN_Surface*)VTReady, 
      (const MDFN_Rect*)VTDRReady,       
      (const MDFN_Rect*)VTLWReady );
  if( success )
  {
    refresh();
  }
}

BOOL wii_start_snapshot()
{
  BOOL succeeded = MDFNI_LoadState( NULL, NULL );                    
  if( !succeeded )
  {
    wii_set_status_message(
      "Error loading the specified save state file." );                
  }
  else
  {
    wii_resume_emulation(); 
  }

  return TRUE;
}