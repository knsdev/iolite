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
	void*            memory_allocate(size_t size);
	void             memory_free(void* pMemory);
	void*            memory_realloc(void* pMemory, size_t newSize);

	template<typename T>
	void memory_delete(T* pMemory)
	{
		pMemory->~T();
		memory_free(pMemory);
	}

	template<typename T>
	T* memory_new_array(size_t count)
	{
		size_t* ptr = (size_t*)memory_allocate(sizeof(size_t) + sizeof(T) * count);
		*ptr = count;
		ptr++;
		T* arr = (T*)ptr;
		return arr;
	}

	template<typename T>
	void memory_delete_array(T* arr)
	{
		size_t* ptr = (size_t*)arr;
		ptr--;
		size_t count = *ptr;

		for (size_t i = 0; i < count; i++)
			(arr + i)->~T();

		memory_free(ptr);
	}

#ifndef IOL_MASTER

	struct AllocationInfo
	{
		bool slotUsed;
		void* pMemory;
		size_t size;
		char fileLineString[512];
	};

	extern AllocationInfo g_allocations[MEMORY_MAX_TRACKED_ALLOCATIONS];

	void             memory_alloc_info_set(AllocationInfo* pInfo, const char* pFile, size_t line, void* _pMemory, size_t _size);
	void             memory_alloc_info_clear(AllocationInfo* pInfo);
	AllocationInfo*  memory_alloc_info_get_free();
	AllocationInfo*  memory_alloc_info_get(void* pMemory);
	size_t           memory_alloc_info_get_all(AllocationInfo* pOutAllocations, size_t outMaxAllocations);
	void             memory_alloc_info_add(const char* pFile, size_t line, void* pMemory, size_t size);
	void             memory_alloc_info_remove(const char* pFile, size_t line, void* pMemory);
	void*            memory_alloc_helper(const char* pFile, size_t line, size_t size);
	void             memory_free_helper(const char* pFile, size_t line, void* pMemory);

	template<typename T>
	T* memory_new_array_helper(const char* pFile, size_t line, size_t count)
	{
		T* arr = memory_new_array<T>(count);
		memory_alloc_info_add(pFile, line, arr, sizeof(T) * count);
		return arr;
	}

	template<typename T>
	void memory_delete_array_helper(const char* pFile, size_t line, T* arr)
	{
		memory_alloc_info_remove(pFile, line, arr);
		memory_delete_array(arr);
	}

	template<typename T>
	void memory_delete_helper(const char* pFile, size_t line, T* pMemory)
	{
		memory_alloc_info_remove(pFile, line, pMemory);
		memory_delete(pMemory);
	}

#endif // !IOL_MASTER
}

#endif // MEMORY_INTERNAL_H
