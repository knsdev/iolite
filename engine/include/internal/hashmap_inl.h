#ifndef IOL_HASHMAP_INL_H
#define IOL_HASHMAP_INL_H

#include "iol_hashmap.h"
#include "iol_memory.h"

namespace iol
{
	template<typename TKey, typename TValue>
	Hashmap<TKey, TValue>::Hashmap()
	{
		m_hashFunction = nullptr;
		m_lists = nullptr;
		m_numLists = 0;
		m_count = 0;
	}

	template<typename TKey, typename TValue>
	Hashmap<TKey, TValue>::~Hashmap()
	{
		Destroy();
	}

	template<typename TKey, typename TValue>
	void Hashmap<TKey, TValue>::Create(uint32(*hashFunction)(const TKey& key), size_t capacity)
	{
		m_hashFunction = hashFunction;
		m_numLists = capacity;
		m_lists = iol_alloc_array(Element*, capacity);
		memory_FillZero(m_lists, sizeof(Element*) * capacity);
	}

	template<typename TKey, typename TValue>
	void Hashmap<TKey, TValue>::Destroy()
	{
		for (size_t i = 0; i < m_numLists; i++)
		{
			Element* element = m_lists[i];

			while (element)
			{
				Element* prev = element;
				element = element->pNext;
				iol_free(prev);
			}
		}

		iol_free(m_lists);
		m_numLists = 0;
		m_count = 0;
	}

	template<typename TKey, typename TValue>
	void Hashmap<TKey, TValue>::Add(const TKey& key, const TValue& value)
	{
		uint32 listIndex = m_hashFunction(key) % m_numLists;
		
		if (m_lists[listIndex] == nullptr)
		{
			Element* newElement = iol_alloc(Element);
			newElement->pNext = nullptr;
			newElement->key = key;
			newElement->value = value;

			m_lists[listIndex] = newElement;
			m_count++;
		}
		else
		{
			Element* current = m_lists[listIndex];

			if (current->key == key)
			{
				current->value = value;
			}
			else
			{
				while (current->pNext)
				{
					current = current->pNext;
				}

				Element* newElement = iol_alloc(Element);
				newElement->pNext = nullptr;
				newElement->key = key;
				newElement->value = value;

				current->pNext = newElement;
				m_count++;
			}
		}
	}

	template<typename TKey, typename TValue>
	bool Hashmap<TKey, TValue>::Remove(const TKey& key)
	{
		uint32 listIndex = m_hashFunction(key) % m_numLists;

		if (m_lists[listIndex] == nullptr)
			return false;

		Element* current = m_lists[listIndex];
		Element* prev = nullptr;

		do 
		{
			if (current->key == key)
			{
				if (prev)
					prev->pNext = current->pNext;

				iol_free(current);

				if (!prev && !current->pNext)
				{
					m_lists[listIndex] = nullptr;
				}

				iol_assert(m_count > 0);
				m_count--;
				return true;
			}

			prev = current;
			current = current->pNext;
		} while (current);

		return false;
	}

	template<typename TKey, typename TValue>
	TValue* Hashmap<TKey, TValue>::Get(const TKey& key) const
	{
		uint32 listIndex = m_hashFunction(key) % m_numLists;
		Element* current = m_lists[listIndex];

		while (current)
		{
			if (current->key == key)
			{
				return &current->value;
			}

			current = current->pNext;
		}

		return nullptr;
	}

	template<typename TKey, typename TValue>
	typename Hashmap<TKey, TValue>::Iterator Hashmap<TKey, TValue>::GetIterator() const
	{
		Hashmap<TKey, TValue>::Iterator it;
		it.listIndex = 0;
		it.element = nullptr;
		it.lists = m_lists;
		it.numLists = m_numLists;

		for (size_t i = 0; i < m_numLists; i++)
		{
			if (m_lists[i])
			{
				it.listIndex = i;
				it.element = m_lists[i];
				return it;
			}
		}

		return it;
	}

	template<typename TKey, typename TValue>
	TValue& Hashmap<TKey, TValue>::Iterator::GetValue() const
	{
		return element->value;
	}

	template<typename TKey, typename TValue>
	TKey& Hashmap<TKey, TValue>::Iterator::GetKey() const
	{
		return element->key;
	}

	template<typename TKey, typename TValue>
	bool Hashmap<TKey, TValue>::Iterator::Increment()
	{
		if (element->pNext)
		{
			element = element->pNext;
			return true;
		}
		
		do
		{
			listIndex++;
		} while (listIndex < numLists && lists[listIndex] == nullptr);

		if (listIndex < numLists)
		{
			element = lists[listIndex];
			return true;
		}
		
		element = nullptr;
		return false;
	}

	template<typename TKey, typename TValue>
	bool Hashmap<TKey, TValue>::Iterator::IsValid() const
	{
		return element != nullptr;
	}
}

#endif // IOL_HASHMAP_INL_H
