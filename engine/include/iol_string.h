#ifndef IOLITE_STRING_H
#define IOLITE_STRING_H

#include "iol_definitions.h"
#include "iol_memory.h"

IOL_API char*			string_Create(size_t capacity);
IOL_API uint32			string_GetHash(const char* pString);
IOL_API size_t			string_GetLength(const char* pString);
IOL_API const char*		string_GetNextLine(const char* pString);
IOL_API char*			string_GetNextLine(char* pString);
IOL_API const char*		string_Find(const char* pString, const char* pStringToFind);
IOL_API char*			string_Find(char* pString, const char* pStringToFind);
IOL_API const char*		string_FindReverse(const char* pString, const char* pStringToFind, size_t skipCount);
IOL_API char*			string_FindReverse(char* pString, const char* pStringToFind, size_t skipCount);
IOL_API bool			string_Contains(const char* pString, const char* pStringToFind);
IOL_API void			string_RemoveCharacters(char* pOutStringBuffer, const char* pSourceString, const char* pCharactersToRemove);
IOL_API char*			string_ReplaceCharacters(char* pString, const char* pCharactersToReplace, char replacementCharacter);
IOL_API char*			string_Copy(char* pStringBuffer, const char* pSourceString, size_t capacity);
IOL_API char*			string_CopyAlloc(const char* pSourceString);
IOL_API bool			string_Compare(const char* pString0, const char* pString1);
IOL_API const char*		string_Skip(const char* pString, const char* charsToSkip);
IOL_API char*			string_Skip(char* pString, const char* charsToSkip);
IOL_API char*			string_SubstringAlloc(const char* pStart, const char* pEnd);
IOL_API void			string_Substring(const char* pStart, const char* pEnd, char* pOutStringBuffer, size_t capacity);
IOL_API char*			string_ConcatAlloc(const char* pStringA, const char* pStringB);

#endif // IOLITE_STRING_H
