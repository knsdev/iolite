#ifndef IOLITE_STRING_H
#define IOLITE_STRING_H

#include "iol_definitions.h"
#include "iol_memory.h"

namespace iol
{
	namespace string
	{
		char*        Create(size_t capacity);
		uint32       GetHash(const char* pString);
		size_t       GetLength(const char* pString);
		const char*  GetNextLine(const char* pString);
		char*        GetNextLine(char* pString);
		const char*  Find(const char* pString, const char* pStringToFind);
		char*        Find(char* pString, const char* pStringToFind);
		const char*  FindReverse(const char* pString, const char* pStringToFind, size_t skipCount);
		char*        FindReverse(char* pString, const char* pStringToFind, size_t skipCount);
		bool         Contains(const char* pString, const char* pStringToFind);
		void         RemoveCharacters(char* pOutStringBuffer, const char* pSourceString, const char* pCharactersToRemove);
		char*        ReplaceCharacters(char* pString, const char* pCharactersToReplace, char replacementCharacter);
		char*        Copy(char* pStringBuffer, const char* pSourceString, size_t capacity);
		char*        CopyAlloc(const char* pSourceString);
		bool         Compare(const char* pString0, const char* pString1);
		const char*  Skip(const char* pString, const char* charsToSkip);
		char*        Skip(char* pString, const char* charsToSkip);
		char*        SubstringAlloc(const char* pStart, const char* pEnd);
		void         Substring(const char* pStart, const char* pEnd, char* pOutStringBuffer, size_t capacity);
		char*        ConcatAlloc(const char* pStringA, const char* pStringB);
	}
}

#endif // IOLITE_STRING_H
