#include "main.h"

#include <ogc/machine/asm.h>
#include <ogc/system.h>
#include <ogc/machine/processor.h>
#include <string.h>

#include "mem2.h"

#ifdef WII_NETTRACE
#include <network.h>
#include "net_print.h"  
#endif

static u8* mem2_ptr = NULL;
static u32 mem2_size = 0;
static u8* head = NULL;
static u8* last_head = NULL;
static u32 last_size = 0;

void InitMem2Manager()
{
  int size = (32*1024*1024)+(256*1024)+32;
  u32 level;
  _CPU_ISR_Disable(level);
  mem2_ptr = (u8*)((u32)SYS_GetArena2Hi()-size);
  SYS_SetArena2Hi(mem2_ptr); 
  _CPU_ISR_Restore(level);
  mem2_size = size;
  Mem2ManagerReset();

#ifdef WII_NETTRACE
  net_print_string( NULL, 0, "InitMem2Manager: %d, %u\n", mem2_size, mem2_ptr );
#endif
}

void Mem2ManagerReset()
{
  //memset( mem2_ptr, 0x0, mem2_size );
  head = last_head = (u8*)(((u32)mem2_ptr+0x1f)&(~0x1f)); // Align to 32 bytes
  last_size = 0;
}

u8* Mem2ManagerAlloc( u32 size, const char* purpose )
{
  if( (((u32)head + size)-(u32)mem2_ptr) > mem2_size )
  {
    MDFN_PrintError( "Unable to allocate '%s': %u bytes", purpose, size );
    return NULL;
  }
  else
  {
    last_head = head;
    last_size = size;
    head += size;
    head = (u8*)(((u32)head+0x1f)&(~0x1f)); // Align to 32 bytes

#ifdef WII_NETTRACE
  net_print_string( NULL, 0, "Mem2ManagerAlloc: %s = 0x%x, %0.2f\n", 
    purpose, last_head, ((float)(size)/1048576.0) );
#endif
    memset( last_head, 0x0, size );
    return last_head;
  }
}

u8* Mem2ManagerCalloc( int count, u32 size, const char* purpose )
{
#ifdef WII_NETTRACE
  net_print_string( NULL, 0, "Mem2ManagerCalloc: %d 0x%x %s\n", count, size, purpose );
#endif

  u32 realSize = size * count;
  u8* result = Mem2ManagerAlloc( realSize, purpose );
  //if( result != NULL )
  //{
  //  memset( result, 0x0, realSize );
  //}

  return result;
}

u8* Mem2ManagerAdjust( u8* mem, u32 size, const char* purpose )
{
  if( mem != last_head )
  {
    MDFN_PrintError( "Unable to adjust '%s', not last allocation.", purpose );
    return NULL;
  }
  else if( (((u32)last_head + size)-(u32)mem2_ptr) > mem2_size )
  {
    MDFN_PrintError( "Unable to adjust '%s': %u bytes", purpose, size );
    return NULL;
  }
 
  head = last_head + size;
  head = (u8*)(((u32)head+0x1f)&(~0x1f)); // Align to 32 bytes
  last_size = size;
#ifdef WII_NETTRACE
  net_print_string( NULL, 0, "Mem2ManagerAdjust: %s = 0x%x, %0.2f\n", 
    purpose, last_head, ((float)(size)/1048576.0) );
#endif
  return last_head;
}