#ifndef IOLITE_ARRAY_H
#define IOLITE_ARRAY_H

#include "iol_definitions.h"
#include "iol_debug.h"
#include "iol_memory.h"
#include "iol_core.h"

namespace iol
{
	template<typename T>
	struct Array
	{
		T*		pData;
		size_t	capacity;
		size_t	count;

		Array();
		Array(size_t _capacity);
		~Array();

		void	Create(size_t _capacity);
		void	Destroy();
		void	Clear();
		T&		PushBack();
		T&		PushBack(const T& element);
		T*		PushBackArray(const T* pElements, size_t count);
		void	PopBack();
		void	Remove(const T& element);
		void	RemoveUnordered(const T& element);
		void	RemoveAt(size_t index);
		void	RemoveAtUnordered(size_t index);
		bool	IsFull();

		T& operator[] (size_t index) { iol_assert(index < count); return pData[index]; }
		const T& operator[] (size_t index) const { iol_assert(index < count); return pData[index]; }
	};
}

#include "internal/array_inl.h"

#endif // IOLITE_ARRAY_H
