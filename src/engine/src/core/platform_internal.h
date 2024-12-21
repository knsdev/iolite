#ifndef PLATFORM_INTERNAL_H
#define PLATFORM_INTERNAL_H

#ifdef IOL_PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace iol
{
	bool platform_IsResultFailure(HRESULT result);
}

#elif IOL_PLATFORM_LINUX

#include <syslog.h>

#else

#endif

#endif // PLATFORM_INTERNAL_H