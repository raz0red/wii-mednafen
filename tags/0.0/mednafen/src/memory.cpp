/* Mednafen - Multi-system Emulator
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "mednafen.h"

#include <stdlib.h>
#include <errno.h>

#include "memory.h"

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"  
#endif

void *MDFN_calloc_real(uint32 nmemb, uint32 size, const char *purpose, const char *_file, const int _line)
{
  void *ret;

  ret = calloc(nmemb, size);

#ifdef WII_NETTRACE
  net_print_string( NULL, 0, "MDFN_calloc_real: %s = 0x%x, %s, %0.2f\n", 
    purpose, ret, ( ((u32)ret) < 0x90000000 ? "MEM1" : "MEM2" ), ((float)(size*nmemb)/1048576.0) );
#endif

  if(!ret)
  {
    MDFN_PrintError(_("Error allocating(calloc) %u bytes for \"%s\" in %s(%d)!"), size, purpose, _file, _line);
    return(0);
  }
  return ret;
}

void *MDFN_malloc_real(uint32 size, const char *purpose, const char *_file, const int _line)
{
  void *ret;

  ret = malloc(size);

#ifdef WII_NETTRACE
  net_print_string( NULL, 0, "MDFN_malloc_real: %s = 0x%x, %s, %0.2f\n", 
    purpose, ret, ( ((u32)ret) < 0x90000000 ? "MEM1" : "MEM2" ), ((float)size/1048576.0) );
#endif

  if(!ret)
  {
    MDFN_PrintError(_("Error allocating(malloc) %u(%0.2f) bytes for \"%s\" in %s(%d)!"), size, ((float)size/1048576.0), purpose, _file, _line);
    return(0);
  }
  return ret;
}

void *MDFN_realloc_real(void *ptr, uint32 size, const char *purpose, const char *_file, const int _line)
{
  void *ret;

  ret = realloc(ptr, size);

#ifdef WII_NETTRACE
  net_print_string( NULL, 0, "MDFN_realloc_real: %s = 0x%x, %s, %0.2f\n", 
    purpose, ret, ( ((u32)ret) < 0x90000000 ? "MEM1" : "MEM2" ), ((float)size/1048576.0) );
#endif

  if(!ret)
  {
    MDFN_PrintError(_("Error allocating(realloc) %u bytes for \"%s\" in %s(%d)!"), size, purpose, _file, _line);
    return(0);
  }
  return ret;
}

void MDFN_free_real(void *ptr, const char *_file, const int _line)
{
#ifdef WII_NETTRACE
  net_print_string( NULL, 0, "MDFN_free: 0x%x %s(%d)\n", ptr, _file, _line );
#endif

  free(ptr);
}

