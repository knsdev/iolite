#include "iol_debug.h"
#include "iol_file.h"
#include "platform_internal.h"
#include <stdarg.h>
#include <mutex>

namespace iol
{
	static std::mutex s_debugMutex;
	static const char* s_errorFilePath = "./log/error.txt";
	static char s_debugStringBuffer[1024 * 1024];

	void debug_output_string(const char* pFormat, ...)
	{
		std::lock_guard<std::mutex> mutex(s_debugMutex);

		va_list args;
		va_start(args, pFormat);
		vsnprintf(s_debugStringBuffer, sizeof(s_debugStringBuffer), pFormat, args);
		va_end(args);

#ifdef IOL_PLATFORM_WINDOWS
		OutputDebugString(s_debugStringBuffer);
#elif defined(IOL_PLATFORM_LINUX)
		printf(s_debugStringBuffer);
#endif
	}

	void debug_output_string_to_log_file(const char* pFormat, ...)
	{
		std::lock_guard<std::mutex> mutex(s_debugMutex);

		va_list args;
		va_start(args, pFormat);
		vsnprintf(s_debugStringBuffer, sizeof(s_debugStringBuffer), pFormat, args);
		va_end(args);

		size_t stringLength = strnlen(s_debugStringBuffer, sizeof(s_debugStringBuffer));

		File* file = file_Open(s_errorFilePath, FileMode::BinaryAppend);
		file_Write(file, s_debugStringBuffer, stringLength);
		file_Close(file);
	}
}