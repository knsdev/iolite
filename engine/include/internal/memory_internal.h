#ifndef MEMORY_INTERNAL_H
#define MEMORY_INTERNAL_H

#include "iol_definitions.h"

#ifndef MEMORY_MAX_TRACKED_ALLOCATIONS
#define MEMORY_MAX_TRACKED_ALLOCATIONS 100 * 1024
#endif

#ifndef MEMORY_ALIGNMENT_DEFAULT
#define MEMORY_ALIGNMENT_DEFAULT 16
#endif

namespace iol
{
	IOL_API void*	memory_allocate(size_t size);
	IOL_API void	memory_free(void* pMemory);
	IOL_API void*	memory_realloc(void* pMemory, size_t newSize);

#ifndef IOL_MASTER
	struct AllocationInfo
	{
		bool slotUsed;
		void* pMemory;
		size_t size;
		char fileLineString[512];
	};

	extern AllocationInfo g_allocations[MEMORY_MAX_TRACKED_ALLOCATIONS];

	void				memory_alloc_info_set(AllocationInfo* pInfo, const char* pFile, size_t line, void* _pMemory, size_t _size);
	void				memory_alloc_info_clear(AllocationInfo* pInfo);
	AllocationInfo*		memory_alloc_info_get_free();
	AllocationInfo*		memory_alloc_info_get(void* pMemory);
	size_t				memory_alloc_info_get_all(AllocationInfo* pOutAllocations, size_t outMaxAllocations);
	IOL_API void		memory_alloc_info_add(const char* pFile, size_t line, void* pMemory, size_t size);
	IOL_API void		memory_alloc_info_remove(const char* pFile, size_t line, void* pMemory);
	IOL_API void*		memory_alloc_helper(const char* pFile, size_t line, size_t size);
	IOL_API void		memory_free_helper(const char* pFile, size_t line, void* pMemory);
#endif // !IOL_MASTER
}

#endif // MEMORY_INTERNAL_H
