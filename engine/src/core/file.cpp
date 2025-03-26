#include "iol_file.h"
#include "iol_debug.h"
#include "iol_memory.h"
#include "iol_string.h"

#include "SDL.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace iol
{
	struct File
	{
		FILE* handle;
		FileMode mode;
		size_t position;
	};

	static const char* s_fileModes[] =
	{
		"rb",	// FileMode_BinaryRead
		"wb",	// FileMode_BinaryWrite
		"wb+",	// FileMode_BinaryWriteUpdate
		"ab"	// FileMode_BinaryAppend
	};

	static const char* file_GetModeString(FileMode fileMode)
	{
		return s_fileModes[(size_t)fileMode];
	}

	bool file_CanRead(FileMode fileMode)
	{
		return (fileMode == FileMode::BinaryRead);
	}

	bool file_CanReposition(FileMode fileMode)
	{
		return (fileMode != FileMode::BinaryAppend);
	}

	bool file_CanWrite(FileMode fileMode)
	{
		return (fileMode == FileMode::BinaryWrite
			|| fileMode == FileMode::BinaryWriteUpdate
			|| fileMode == FileMode::BinaryAppend);
	}

	File* file::Open(const char* pFilePath, FileMode mode)
	{
#ifdef IOL_PLATFORM_WINDOWS
#pragma warning ( push )
#pragma warning (disable: 4996)
#endif

		iol_assert(pFilePath != nullptr);

		File* file = iol_alloc(File);
		file->mode = mode;
		file->position = 0u;
		file->handle = fopen(pFilePath, file_GetModeString(mode));

		if (file->handle == nullptr)
		{
			if (file_CanWrite(mode))
			{
				// Try to create all directories for the filePath
				static char s_directoryPath[1024];
				file::GetDirectoryPath(pFilePath, s_directoryPath, sizeof(s_directoryPath));

#define IOL_MKDIR_CMD_CAPACITY 2048
				static char mkdirCommand[IOL_MKDIR_CMD_CAPACITY];

#ifdef IOL_PLATFORM_WINDOWS
				sprintf(mkdirCommand, "mkdir %s", s_directoryPath);
				char* pMkdir = mkdirCommand;
				while (*pMkdir)
				{
					if (*pMkdir == '/')
						*pMkdir = '\\';
					++pMkdir;
				}
#elif defined(IOL_PLATFORM_LINUX)
				snprintf(mkdirCommand, IOL_MKDIR_CMD_CAPACITY, "mkdir -p %s", s_directoryPath);
#else
#error not implemented
#endif

				if (system(mkdirCommand) == 0)
				{
					file->handle = fopen(pFilePath, file_GetModeString(mode));
				}
			}
		}

		if (file->handle == nullptr)
		{
			iol_log_error("failed to open file: %s", pFilePath);
			iol_free(file);
			return nullptr;
		}

#ifdef IOL_PLATFORM_WINDOWS
#pragma warning ( pop )
#endif

		return file;
	}

	void file::Close(File* file)
	{
		if (file->handle != nullptr)
		{
			fclose(file->handle);
			file->handle = nullptr;
		}

		iol_free(file);
	}

	size_t file::Read(File* file, void* pBuffer, size_t size)
	{
		iol_assert(pBuffer != nullptr);
		iol_assert(size > 0);
		iol_assert(file_CanRead(file->mode));

		size_t numBytesRead = fread(pBuffer, 1, size, file->handle);
		file->position += numBytesRead;

		return numBytesRead;
	}

	bool file::Write(File* file, const void* pData, size_t size)
	{
		iol_assert(pData != nullptr);
		iol_assert(size > 0);
		iol_assert(file_CanWrite(file->mode));

		size_t numBytesWritten = fwrite(pData, 1, size, file->handle);
		file->position += numBytesWritten;

		return numBytesWritten > 0;
	}

	size_t file::GetSize(File* file)
	{
		iol_assert(file_CanReposition(file->mode));

		size_t fileSize;
		long result;

		result = fseek(file->handle, 0, SEEK_END);
		if (result != 0)
		{
			return 0;
		}

		result = ftell(file->handle);
		if (result < 0)
		{
			return 0;
		}

		fileSize = (size_t)result;

		result = fseek(file->handle, (long)file->position, SEEK_SET);
		iol_assert(result == 0);

		return fileSize;
	}

	bool file::SetPosition(File* file, size_t position)
	{
		iol_assert(file_CanReposition(file->mode));

		if (fseek(file->handle, (long)position, SEEK_SET) == 0)
		{
			file->position = position;
			return true;
		}

		return false;
	}

	bool file::SetPositionSpecial(File* file, SpecialFilePosition position)
	{
		iol_assert(file_CanReposition(file->mode));

		bool result;

		switch (position)
		{
		case SpecialFilePosition::Start:
			result = fseek(file->handle, 0, SEEK_SET) == 0;
			break;
		case SpecialFilePosition::End:
			result = fseek(file->handle, 0, SEEK_END) == 0;
			break;
		default:
			iol_breakpoint;
			return false;
		}

		if (result)
		{
			file->position = (size_t)(ftell(file->handle));
			return true;
		}

		iol_breakpoint;
		return false;
	}

	size_t file::GetPosition(const File* file)
	{
		return file->position;
	}

	bool file::GetDirectoryPath(const char* pFilePath, char* pDirectoryPath, size_t directoryPathSize)
	{
		pDirectoryPath[0] = '\0';

		const char* pDirectoryStart;
		const char* pDirectoryEnd;

		while (*pFilePath == '.')
		{
			++pFilePath;
		}
		if (*pFilePath == '/')
		{
			++pFilePath;
		}

		pDirectoryStart = pFilePath;

		if (pDirectoryStart != nullptr)
		{
			pDirectoryEnd = nullptr;
			const char* pCurrentEnd = pDirectoryStart;

			do
			{
				pCurrentEnd = strstr(pCurrentEnd + 1, "/");
				if (pCurrentEnd != nullptr)
				{
					pDirectoryEnd = pCurrentEnd;
				}
			} while (pCurrentEnd != nullptr);

			if (pDirectoryEnd != nullptr)
			{
				string::Substring(pDirectoryStart, pDirectoryEnd, pDirectoryPath, directoryPathSize);
				return true;
			}
		}

		return false;
	}

	void* file::ReadAll(const char* pFilePath, size_t* pOutFileSize, size_t extraBufferSize)
	{
		*pOutFileSize = 0u;

		File* pFile = file::Open(pFilePath, FileMode::BinaryRead);

		if (pFile == nullptr)
			return nullptr;

		size_t size = file::GetSize(pFile);

		if (size == 0u)
			return nullptr;

		void* pBuffer = iol_alloc_raw(size + extraBufferSize);
		size_t bytesRead = file::Read(pFile, pBuffer, size);

		*pOutFileSize = bytesRead;

		file::Close(pFile);

		return pBuffer;
	}

	char* file::ReadAllText(const char* pFilePath, size_t* pOutFileSize, size_t extraBufferSize)
	{
		if (pOutFileSize)
			*pOutFileSize = 0u;

		size_t bytesRead = 0u;
		char* pBuffer = (char*)file::ReadAll(pFilePath, &bytesRead, 1u + extraBufferSize);

		if (pBuffer == nullptr)
			return nullptr;

		pBuffer[bytesRead] = '\0';

		if (pOutFileSize)
			*pOutFileSize = bytesRead;

		return pBuffer;
	}
}