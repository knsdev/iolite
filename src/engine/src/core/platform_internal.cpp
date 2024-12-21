#include "platform_internal.h"
#include "iol_debug.h"

namespace iol
{
#ifdef IOL_PLATFORM_WINDOWS
	bool platform_IsResultFailure(HRESULT result)
	{
		if (FAILED(result))
		{
			static char s_buffer[4096];
			size_t numChars = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, (DWORD)result, LANG_USER_DEFAULT, (LPTSTR)&s_buffer, 0, nullptr);
			iol_assert(numChars < sizeof(s_buffer));
			s_buffer[numChars] = 0;

			iol_log_raw_error("[windows_internal] hresult: 0x%x , %s", result, s_buffer);
			return true;
		}

		return false;
	}
#endif
}

