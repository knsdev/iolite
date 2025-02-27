#ifndef IOL_HASHMAP_H
#define IOL_HASHMAP_H

#include "iol_definitions.h"

namespace iol
{
	template<typename TKey, typename TValue>
	class Hashmap
	{
	public:
		struct Element
		{
			Element* pNext;
			TKey key;
			TValue value;
		};

		class Iterator
		{
		public:
			bool     Increment();
			bool     IsValid() const;
			TKey&    GetKey() const;
			TValue&  GetValue() const;

			size_t listIndex;
			Element* element;

			Element** lists;
			size_t numLists;
		};

	public:
		Hashmap();
		~Hashmap();

		Hashmap(const Hashmap& other) = delete;
		Hashmap& operator=(const Hashmap& other) = delete;
		Hashmap(Hashmap&& other) = delete;
		Hashmap& operator=(Hashmap&& other) = delete;

		void      Create(uint32(*hashFunction)(const TKey& key), size_t capacity);
		void      Destroy();

		void      Add(const TKey& key, const TValue& value);
		bool      Remove(const TKey& key);
		TValue*   Get(const TKey& key) const;

		size_t    GetCount() const { return m_count; }
		Iterator  GetIterator() const;

	private:
		uint32(*m_hashFunction)(const TKey& key);
		Element** m_lists;
		size_t m_numLists;
		size_t m_count;
	};
}

#include "internal/hashmap_inl.h"

#endif // IOL_HASHMAP_H