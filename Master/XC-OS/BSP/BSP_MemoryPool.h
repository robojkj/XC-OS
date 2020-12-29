#ifndef __BSP_MemoryPool_H
#define __BSP_MemoryPool_H

//#include "Arduino.h"

#include "stdint.h"


#ifdef __cplusplus
extern "C" {
#endif

/*MemoryPool*/
void* MemPool_Malloc(uint32_t size);
void MemPool_Free(void* pData);

#ifdef __cplusplus
}
#endif

#endif


