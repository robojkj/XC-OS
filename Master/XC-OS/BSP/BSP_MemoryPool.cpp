#include "BSP.h"
#include "BSP_MemoryPool.h"

typedef struct{
    void *pData;
    uint32_t Size;
}UseRecord_TypeDef;

static const uint32_t MemoryPoolSize = 50 * 1024;
static uint8_t MemoryPoolBuff[MemoryPoolSize];
static uint32_t MemoryPoolTop = 0;

static const uint8_t RecordListSize = 20;
static UseRecord_TypeDef MemoryPoolUseRecordList[RecordListSize];
static uint8_t RecordListIndex = 0;

static uint32_t RecordList_GetDataSize(void* pData)
{
    if(pData == NULL)
        return 0;
    
    uint32_t size = 0;
    for(int i = 0; i < RecordListIndex; i++)
    {
        if(pData == MemoryPoolUseRecordList[i].pData)
        {
            size = MemoryPoolUseRecordList[i].Size;
            break;
        }
    }
    return size;
}

static bool RecordList_AddNewData(void* pData, uint32_t size)
{
    if(pData == NULL)
        return false;
    
    if(RecordListIndex >= RecordListSize - 1)
        return false;
    
    if(RecordList_GetDataSize(pData) == 0)
    {
        MemoryPoolUseRecordList[RecordListIndex].pData = pData;
        MemoryPoolUseRecordList[RecordListIndex].Size = size;
        RecordListIndex++;
        return true;
    }
    
    return false;
}

void* MemPool_Malloc(uint32_t size)
{
    uint8_t* pData = NULL;
    if(MemoryPoolTop + size <= MemoryPoolSize)
    {
        uint8_t* p = MemoryPoolBuff + MemoryPoolTop;
        if(RecordList_AddNewData(p, size))
        {
            pData = p;
            MemoryPoolTop += size;
        }
    }
    return pData;
}

void MemPool_Free(void* pData)
{
    if(pData == NULL)
        return;
    
    uint32_t size = RecordList_GetDataSize(pData);
    if(size > 0)
    {
        MemoryPoolTop -= size;
        RecordListIndex--;
        MemoryPoolUseRecordList[RecordListIndex].pData = NULL;
        MemoryPoolUseRecordList[RecordListIndex].Size = 0;
    }
}

uint32_t MemPool_GetResidueSize()
{
    return (MemoryPoolSize - MemoryPoolTop);
}

void MemPool_Clear(uint8_t n)
{
    memset(MemoryPoolBuff, n, MemoryPoolSize);
}

//释放要按顺序，否则会内存泄漏，所以增加内存池所有清空功能
void MemPool_ALLFree()
{
	MemoryPoolTop = 0;
	RecordListIndex = 0;
	
	memset(&MemoryPoolUseRecordList, 0, sizeof(UseRecord_TypeDef) * RecordListSize);
}












