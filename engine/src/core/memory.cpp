#include "iol_memory.h"
#include "iol_core.h"
#include "iol_debug.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

namespace iol
{
	void* memory_allocate(size_t size)
	{
		iol_assert(size > 0);

		void* pMemory = malloc(size);
		iol_assert(pMemory != nullptr);

		return pMemory;
	}

	void memory_free(void* pMemory)
	{
		iol_assert(pMemory != nullptr);
		free(pMemory);
	}

	void* memory_realloc(void* pMemory, size_t newSize)
	{
		iol_assert(pMemory != nullptr);

		return realloc(pMemory, newSize);
	}

#ifndef IOL_MASTER
	AllocationInfo	g_allocations[MEMORY_MAX_TRACKED_ALLOCATIONS];

	void memory_alloc_info_set(AllocationInfo* pInfo, const char* pFile, size_t line, void* _pMemory, size_t _size)
	{
		pInfo->slotUsed = true;
		snprintf(pInfo->fileLineString, sizeof(pInfo->fileLineString), "[%s:%zu]", pFile, line);
		pInfo->pMemory = _pMemory;
		pInfo->size = _size;
	}

	void memory_alloc_info_clear(AllocationInfo* pInfo)
	{
		memory::FillZero(pInfo, sizeof(*pInfo));
	}

	AllocationInfo* memory_alloc_info_get_free()
	{
		for (size_t i = 0; i < MEMORY_MAX_TRACKED_ALLOCATIONS; ++i)
		{
			if (!g_allocations[i].slotUsed)
				return &g_allocations[i];
		}

		return nullptr;
	}

	AllocationInfo* memory_alloc_info_get(void* pMemory)
	{
		if (pMemory == nullptr)
			return nullptr;

		for (size_t i = 0; i < MEMORY_MAX_TRACKED_ALLOCATIONS; ++i)
		{
			if (g_allocations[i].pMemory == pMemory)
				return &g_allocations[i];
		}

		return nullptr;
	}

	size_t memory_alloc_info_get_all(AllocationInfo* pOutAllocations, size_t outMaxAllocations)
	{
		size_t outIndex = 0;
		size_t maxCount = core::Min<size_t>(outMaxAllocations, MEMORY_MAX_TRACKED_ALLOCATIONS);

		for (size_t i = 0; i < maxCount; ++i)
		{
			if (g_allocations[i].slotUsed == true)
			{
				if (outIndex < maxCount)
				{
					pOutAllocations[outIndex++] = g_allocations[i];
				}
				else
				{
					return ++outIndex;
				}
			}
		}

		return outIndex;
	}

	void memory_alloc_info_add(const char* pFile, size_t line, void* pMemory, size_t size)
	{
		//iol::outputDebugString("allocation: file: %s | line: %d\n", pFunction, pFile, line);

		AllocationInfo* pAllocationInfo = memory_alloc_info_get_free();

		// Increase Memory_MaxTrackedAllocations if you hit this
		iol_assert(pAllocationInfo);

		if (pAllocationInfo != nullptr)
			memory_alloc_info_set(pAllocationInfo, pFile, line, pMemory, size);
	}

	void memory_alloc_info_remove(const char* pFile, size_t line, void* pMemory)
	{
		//iol::outputDebugString("deallocation: file: %s | line: %d\n", pFunction, pFile, line);

		AllocationInfo* pAllocationInfo = memory_alloc_info_get(pMemory);

		if (pAllocationInfo != nullptr)
		{
			memory_alloc_info_clear(pAllocationInfo);
		}
	}

	void* memory_alloc_helper(const char* pFile, size_t line, size_t size)
	{
		void* pMemory = memory_allocate(size);
		memory_alloc_info_add(pFile, line, pMemory, size);
		return pMemory;
	}

	void memory_free_helper(const char* pFile, size_t line, void* pMemory)
	{
		memory_alloc_info_remove(pFile, line, pMemory);
		memory_free(pMemory);
	}

	void memory::LogAllocations()
	{
		static AllocationInfo s_allocationInfos[MEMORY_MAX_TRACKED_ALLOCATIONS];
		size_t numAllocations = memory_alloc_info_get_all(s_allocationInfos, iol_countof(s_allocationInfos));

		for (size_t i = 0; i < numAllocations; ++i)
		{
			iol_log_raw_error("[memory] %s | address: %p | size: %zu", s_allocationInfos[i].fileLineString, s_allocationInfos[i].pMemory, s_allocationInfos[i].size);
		}
	}

#endif // !IOL_MASTER

	void memory::FillZero(void* pDestination, size_t size)
	{
		iol_assert(pDestination != nullptr);
		iol_assert(size > 0);

		memset(pDestination, 0, size);
	}

	void memory::Fill(void* pDestination, size_t size, int value)
	{
		iol_assert(pDestination != nullptr);
		iol_assert(size > 0);

		memset(pDestination, value, size);
	}

	void memory::Copy(void* pDestination, size_t size, const void* pSource)
	{
		iol_assert(pDestination != nullptr);
		iol_assert(size > 0);
		iol_assert(pSource != nullptr);

		memcpy(pDestination, pSource, size);
	}

	void memory::Move(void* pDestination, size_t size, const void* pSource)
	{
		iol_assert(pDestination != nullptr);
		iol_assert(size > 0);
		iol_assert(pSource != nullptr);

		memmove(pDestination, pSource, size);
	}

	bool memory::Compare(const void* pFirst, const void* pSecond, size_t size)
	{
		iol_assert(pFirst != nullptr);
		iol_assert(pSecond != nullptr);
		iol_assert(size > 0);

		return memcmp(pFirst, pSecond, size) == 0;
	}
}