#include "iol_string.h"
#include "iol_debug.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

namespace iol
{
	char* string_Create(size_t capacity)
	{
		char* pString = (char*)iol_alloc_raw(capacity);
		return pString;
	}

	uint32 string_GetHash(const char* pString)
	{
		iol_assert(pString);

		uint32 hash = 0;

		const char* pCurrent = pString;
		while (*pCurrent)
		{
			uint8 current = (uint8)(*pCurrent);
			hash = (hash + current * 47) ^ 151;
			++pCurrent;
		}

		return hash;
	}

	size_t string_GetLength(const char* pString)
	{
		iol_assert(pString);

		size_t length = 0;
		while (*pString++)
		{
			++length;
		}

		return length;
	}

	const char* string_GetNextLine(const char* pString)
	{
		while (*pString && *pString != '\n')
		{
			++pString;
		}

		if (*pString)
		{
			++pString;
		}

		return pString;
	}

	char* string_GetNextLine(char* pString)
	{
		while (*pString && *pString != '\n')
		{
			++pString;
		}

		if (*pString)
		{
			++pString;
		}

		return pString;
	}

	const char* string_Find(const char* pString, const char* pStringToFind)
	{
		iol_assert(pString);
		iol_assert(pStringToFind);

		return strstr(pString, pStringToFind);
	}

	char* string_Find(char* pString, const char* pStringToFind)
	{
		iol_assert(pString);
		iol_assert(pStringToFind);

		return strstr(pString, pStringToFind);
	}

	const char* string_FindReverse(const char* pString, const char* pStringToFind, size_t skipCount /* = 0 */)
	{
		size_t s1len = strlen(pString);
		size_t s2len = strlen(pStringToFind);

		if (s2len > s1len)
			return nullptr;

		const char* s;
		size_t resultIdx = 0;

		for (s = pString + s1len - s2len; s >= pString; --s)
		{
			if (strncmp(s, pStringToFind, s2len) == 0)
			{
				if (resultIdx >= skipCount)
					return s;

				resultIdx++;
			}
		}

		return nullptr;
	}

	char* string_FindReverse(char* pString, const char* pStringToFind, size_t skipCount /* = 0 */)
	{
		size_t s1len = strlen(pString);
		size_t s2len = strlen(pStringToFind);

		if (s2len > s1len)
			return nullptr;

		char* s;
		size_t resultIdx = 0;

		for (s = pString + s1len - s2len; s >= pString; --s)
		{
			if (strncmp(s, pStringToFind, s2len) == 0)
			{
				if (resultIdx >= skipCount)
					return s;

				resultIdx++;
			}
		}

		return nullptr;
	}

	bool string_Contains(const char* pString, const char* pStringToFind)
	{
		iol_assert(pString);
		iol_assert(pStringToFind);

		return strstr(pString, pStringToFind) != nullptr;
	}

	void string_RemoveCharacters(char* pOutStringBuffer, const char* pSourceString, const char* pCharactersToRemove)
	{
		iol_assert(pSourceString);
		iol_assert(pCharactersToRemove);
		iol_assert(pOutStringBuffer);

		const char* pCurrent = pSourceString;
		size_t outIndex = 0;
		while (*pCurrent)
		{
			bool remove = false;
			const char* pCurrentCharToRemove = pCharactersToRemove;
			while (*pCurrentCharToRemove)
			{
				if (*pCurrentCharToRemove == *pCurrent)
				{
					remove = true;
					break;
				}
				++pCurrentCharToRemove;
			}
			if (!remove)
			{
				pOutStringBuffer[outIndex] = *pCurrent;
				++outIndex;
			}

			++pCurrent;
		}

		pOutStringBuffer[outIndex] = '\0';
	}

	char* string_ReplaceCharacters(char* pString, const char* pCharactersToReplace, char replacementCharacter)
	{
		char* pCurrent = pString;

		while (*pCurrent)
		{
			const char* pReplaceChar = pCharactersToReplace;

			while (*pReplaceChar)
			{
				if (*pCurrent == *pReplaceChar)
				{
					*pCurrent = replacementCharacter;
					break;
				}

				pReplaceChar++;
			}

			pCurrent++;
		}

		return pString;
	}

	char* string_Copy(char* pStringBuffer, const char* pSourceString, size_t capacity)
	{
		iol_assert(pSourceString);
		iol_assert(pStringBuffer);

		size_t i;
		size_t maxCount = capacity - 1;
		for (i = 0; i < maxCount && pSourceString[i]; i++)
		{
			pStringBuffer[i] = pSourceString[i];
		}

		pStringBuffer[i] = '\0';

		return pStringBuffer;
	}

	char* string_CopyAlloc(const char* pSourceString)
	{
		size_t capacity = string_GetLength(pSourceString) + 1;
		char* pString = string_Create(capacity);
		string_Copy(pString, pSourceString, capacity);

		return pString;
	}

	bool string_Compare(const char* pString0, const char* pString1)
	{
		iol_assert(pString0);
		iol_assert(pString1);

		size_t i;
		for (i = 0; pString0[i] && pString1[i]; ++i)
		{
			if (pString0[i] != pString1[i])
			{
				return false;
			}
		}

		return true;
	}

	const char* string_Skip(const char* pString, const char* charsToSkip)
	{
		while (pString && *pString)
		{
			bool skip = false;

			while (*charsToSkip)
			{
				if (*charsToSkip == *pString)
				{
					skip = true;
					break;
				}

				charsToSkip++;
			}

			if (!skip)
				return pString;

			pString++;
		}

		return pString;
	}

	char* string_Skip(char* pString, const char* charsToSkip)
	{
		while (pString && *pString)
		{
			bool skip = false;

			while (*charsToSkip)
			{
				if (*charsToSkip == *pString)
				{
					skip = true;
					break;
				}

				charsToSkip++;
			}

			if (!skip)
				return pString;

			pString++;
		}

		return pString;
	}

	char* string_SubstringAlloc(const char* pStart, const char* pEnd)
	{
		if (pEnd == nullptr)
			pEnd = pStart + string_GetLength(pStart);

		size_t size = pEnd - pStart + 1;
		char* pResult = string_Create(size);
		string_Copy(pResult, pStart, size);

		return pResult;
	}

	void string_Substring(const char* pStart, const char* pEnd, char* pOutStringBuffer, size_t capacity)
	{
		if (pEnd == nullptr)
			pEnd = pStart + string_GetLength(pStart);

		size_t size = pEnd - pStart + 1;
		iol_assert(size <= capacity);

		string_Copy(pOutStringBuffer, pStart, size);
	}

	char* string_ConcatAlloc(const char* pStringA, const char* pStringB)
	{
		size_t lengthA = string_GetLength(pStringA);
		size_t lengthB = string_GetLength(pStringB);

		size_t capacity = lengthA + lengthB + 1;
		char* pResult = string_Create(capacity);

		string_Copy(pResult, pStringA, capacity);
		strncat(pResult, pStringB, capacity);

		return pResult;
	}
}