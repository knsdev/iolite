#ifndef IOLITE_STRING_H
#define IOLITE_STRING_H

#include "iol_definitions.h"
#include "iol_memory.h"

namespace iol
{
	char*        string_Create(size_t capacity);
	uint32       string_GetHash(const char* pString);
	size_t       string_GetLength(const char* pString);
	const char*  string_GetNextLine(const char* pString);
	char*        string_GetNextLine(char* pString);
	const char*  string_Find(const char* pString, const char* pStringToFind);
	char*        string_Find(char* pString, const char* pStringToFind);
	const char*  string_FindReverse(const char* pString, const char* pStringToFind, size_t skipCount);
	char*        string_FindReverse(char* pString, const char* pStringToFind, size_t skipCount);
	bool         string_Contains(const char* pString, const char* pStringToFind);
	void         string_RemoveCharacters(char* pOutStringBuffer, const char* pSourceString, const char* pCharactersToRemove);
	char*        string_ReplaceCharacters(char* pString, const char* pCharactersToReplace, char replacementCharacter);
	char*        string_Copy(char* pStringBuffer, const char* pSourceString, size_t capacity);
	char*        string_CopyAlloc(const char* pSourceString);
	bool         string_Compare(const char* pString0, const char* pString1);
	const char*  string_Skip(const char* pString, const char* charsToSkip);
	char*        string_Skip(char* pString, const char* charsToSkip);
	char*        string_SubstringAlloc(const char* pStart, const char* pEnd);
	void         string_Substring(const char* pStart, const char* pEnd, char* pOutStringBuffer, size_t capacity);
	char*        string_ConcatAlloc(const char* pStringA, const char* pStringB);
}

#endif // IOLITE_STRING_H
