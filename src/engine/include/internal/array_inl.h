#ifndef IOLITE_ARRAY_INL_H
#define IOLITE_ARRAY_INL_H

#include "iol_array.h"

namespace iol
{
	template<typename T>
	Array<T>::Array()
	{
		pData = nullptr;
		capacity = 0;
		count = 0;
	}

	template<typename T>
	Array<T>::Array(size_t _capacity)
	{
		pData = nullptr;
		capacity = 0;
		count = 0;
		Create(_capacity);
	}

	template<typename T>
	Array<T>::~Array()
	{
		Destroy();
	}

	template<typename T>
	void Array<T>::Create(size_t _capacity)
	{
		count = 0;

		if (_capacity == 0)
			return;

		if (pData == nullptr)
		{
			pData = iol_alloc_array(T, _capacity);
			capacity = _capacity;
		}
		else if (_capacity > capacity)
		{
			Destroy();
			pData = iol_alloc_array(T, _capacity);
			capacity = _capacity;
		}
	}

	template<typename T>
	void Array<T>::Destroy()
	{
		if (pData != nullptr)
		{
			iol_free(pData);
			pData = nullptr;
		}

		capacity = 0;
		count = 0;
	}

	template<typename T>
	void Array<T>::Clear()
	{
		count = 0;
	}

	template<typename T>
	T& Array<T>::PushBack(const T& element)
	{
		iol_assert(count < capacity);

		T& slot = *(pData + count);
		slot = element;
		count++;

		return slot;
	}

	template<typename T>
	T& Array<T>::PushBack()
	{
		iol_assert(count < capacity);

		T& slot = *(pData + count);
		count++;

		return slot;
	}

	template<typename T>
	T* Array<T>::PushBackArray(const T* pElements, size_t a_count)
	{
		iol_assert(count + a_count <= capacity);

		T* pSlot = pData + count;
		T* pFirstElement = pSlot;

		for (size_t i = 0; i < a_count; ++i)
		{
			pSlot[i] = pElements[i];
		}

		count += a_count;

		return pFirstElement;
	}

	template<typename T>
	void Array<T>::PopBack()
	{
		iol_assert(count > 0);

		count--;
	}

	template<typename T>
	void Array<T>::Remove(const T& element)
	{
		for (int i = count - 1; i >= 0; --i)
		{
			if (pData[i] == element)
			{
				size_t numElementsBehind = count - i;

				if (numElementsBehind > 0)
				{
					memory_Move(&pData[i], numElementsBehind * sizeof(T), &pData[i + 1]);
				}

				count--;
			}
		}
	}

	template<typename T>
	void Array<T>::RemoveUnordered(const T& element)
	{
		for (int i = count - 1; i >= 0; --i)
		{
			if (pData[i] == element)
			{
				Swap(&pData[i], &pData[count - 1]);
				count--;
			}
		}
	}

	template<typename T>
	void Array<T>::RemoveAt(size_t index)
	{
		T* pElement = pData + index;

		iol_assert(count > 0);
		iol_assert(pElement >= pData && pElement <= pData + (capacity - 1) * sizeof(T));

		size_t numElementsBehind = (pData + count - 1) - pElement;

		if (numElementsBehind > 0)
		{
			memory_Move(pElement, numElementsBehind * sizeof(T), pElement + 1);
		}

		count--;
	}

	template<typename T>
	void Array<T>::RemoveAtUnordered(size_t index)
	{
		const T* pElement = pData + index;

		iol_assert(count > 0);
		iol_assert(pElement >= pData && pElement <= pData + (capacity - 1) * sizeof(T));

		Swap(pElement, pData + count - 1);
		count--;
	}

	template<typename T>
	bool Array<T>::IsFull()
	{
		return count == capacity;
	}
}

#endif // IOLITE_ARRAY_INL_H
