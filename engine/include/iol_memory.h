#ifndef IOLITE_MEMORY_H
#define IOLITE_MEMORY_H

#include "internal/memory_internal.h"

// TODO: Read call stack to track memory allocations better

#ifndef IOL_MASTER

#define iol_alloc(T)                  (T*)iol::memory_alloc_helper(__FILE__, __LINE__, sizeof(T))
#define iol_alloc_array(T, count)     (T*)iol::memory_alloc_helper(__FILE__, __LINE__, sizeof(T) * (count))
#define iol_alloc_raw(size)           iol::memory_alloc_helper(__FILE__, __LINE__, size)
#define iol_free(pMemory)             do { iol::memory_free_helper(__FILE__, __LINE__, pMemory); } while(0)

#define iol_new(T, ...)               new (iol::memory_alloc_helper(__FILE__, __LINE__, sizeof(T))) T(__VA_ARGS__)
#define iol_delete(pMemory)           \
    do { \
        if (pMemory) { \
            iol::memory_delete_helper(__FILE__, __LINE__, pMemory); \
            pMemory = nullptr; \
        } \
    } while(0)

#define iol_new_array(T, count)               iol::memory_new_array_helper<T>(__FILE__, __LINE__, count)
#define iol_delete_array(pMemory)   \
    do { \
        if (pMemory) { \
            iol::memory_delete_array_helper(__FILE__, __LINE__, pMemory); \
            pMemory = nullptr; \
        } \
    } while(0)

#else

#define iol_alloc(T)                  (T*)iol::memory_allocate(sizeof(T))
#define iol_alloc_array(T, count)     (T*)iol::memory_allocate(sizeof(T) * count)
#define iol_alloc_raw(size)           iol::memory_allocate(size)
#define iol_free(pMemory)             do { iol::memory_free(pMemory); } while(0)

#define iol_new(T, ...)               new (iol::memory_allocate(sizeof(T))) T(__VA_ARGS__)
#define iol_delete(pMemory)           \
    do { \
        if (pMemory) { \
            iol::memory_delete(pMemory); \
            pMemory = nullptr; \
        } \
    } while(0)

#define iol_new_array(T, count)               iol::memory_new_array<T>(count)
#define iol_delete_array(pMemory)   \
    do { \
        if (pMemory) { \
            iol::memory_delete_array(pMemory); \
            pMemory = nullptr; \
        } \
    } while(0)

#endif

namespace iol
{
	namespace memory
	{
#ifndef IOL_MASTER
		void    LogAllocations();
#endif
		void    FillZero(void* pDestination, size_t size);
		void    Fill(void* pDestination, size_t size, int value);
		void    Copy(void* pDestination, size_t size, const void* pSource);
		void    Move(void* pDestination, size_t size, const void* pSource);
		bool    Compare(const void* pFirst, const void* pSecond, size_t size);
	}
}

#endif // IOLITE_MEMORY_H
