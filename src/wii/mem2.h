#ifndef MEM2_H
#define MEM2_H

#include <gctypes.h>

extern void InitMem2Manager();
extern void Mem2ManagerReset();
extern u8* Mem2ManagerAlloc( u32 size, const char* purpose );
extern u8* Mem2ManagerCalloc( int count, u32 size, const char* purpose );
extern u8* Mem2ManagerAdjust( u8* mem, u32 size, const char* purpose );

#endif