#ifndef IOLITE_MEMORY_H
#define IOLITE_MEMORY_H

#include "internal/memory_internal.h"

// TODO: Read call stack to track memory allocations better

#ifndef IOL_MASTER
#define iol_alloc(T)                  (T*)iol::memory_alloc_helper(__FILE__, __LINE__, sizeof(T))
#define iol_alloc_array(T, count)     (T*)iol::memory_alloc_helper(__FILE__, __LINE__, sizeof(T) * (count))
#define iol_alloc_raw(size)           iol::memory_alloc_helper(__FILE__, __LINE__, size)
#define iol_free(pMemory)             do { iol::memory_free_helper(__FILE__, __LINE__, pMemory); } while(0)
#else
#define iol_alloc(T)                  (T*)iol::memory_allocate(sizeof(T))
#define iol_alloc_array(T, count)     (T*)iol::memory_allocate(sizeof(T) * count)
#define iol_alloc_raw(size)           iol::memory_allocate(size)
#define iol_free(pMemory)             do { iol::memory_free(pMemory); } while(0)
#endif

namespace iol
{
	namespace memory
	{
#ifndef IOL_MASTER
		void    log_allocations();
#endif
		void    FillZero(void* pDestination, size_t size);
		void    Fill(void* pDestination, size_t size, int value);
		void    Copy(void* pDestination, size_t size, const void* pSource);
		void    Move(void* pDestination, size_t size, const void* pSource);
		bool    Compare(const void* pFirst, const void* pSecond, size_t size);
	}
}

#endif // IOLITE_MEMORY_H
