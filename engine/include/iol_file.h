#ifndef IOLITE_FILE_H
#define IOLITE_FILE_H

#include "iol_definitions.h"

namespace iol
{
	enum class FileMode
	{
		BinaryRead,
		BinaryWrite,
		BinaryWriteUpdate,
		BinaryAppend,
	};

	enum class SpecialFilePosition
	{
		Start,
		End,
	};

	struct File;

	namespace file
	{
		File*   Open(const char* filePath, FileMode mode);
		void    Close(File* file);
		size_t  Read(File* file, void* buffer, size_t size);
		bool    Write(File* file, const void* data, size_t size);
		size_t  GetSize(File* file);
		bool    SetPosition(File* file, size_t position);
		bool    SetPositionSpecial(File* file, SpecialFilePosition position);
		size_t  GetPosition(const File* file);

		/*
		* Returns a newly allocated buffer with read file size bytes.
		* Writes the read file size into 'pOutFileSize'.
		* You can increase the buffer size by setting 'extraBufferSize' > 0.
		* 'outFileSize' is not affected by 'additionalBufferSize'.
		* Make sure to free the buffer when you're done!
		*/
		void*   ReadAll(const char* filePath, size_t* outFileSize, size_t extraBufferSize);

		/*
		* Returns a newly allocated buffer with read file size + 1 byte for the termination character.
		* 'outFileSize' is without the +1 for the termination character.
		* See 'file_read_all'.
		*/
		char*   ReadAllText(const char* filePath, size_t* outFileSize, size_t extraBufferSize);

		bool    GetDirectoryPath(const char* filePath, char* outDirectoryPath, size_t directoryPathCapacity);
	}
}

#endif // IOLITE_FILE_H