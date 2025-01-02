#ifndef IOLITE_DEFINITIONS_H
#define IOLITE_DEFINITIONS_H

#include "glm/glm.hpp"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef IOL_PLATFORM_WINDOWS
#	ifdef IOL_BUILD_DLL
#		define IOL_API //__declspec(dllexport)
#	else
#		define IOL_API //__declspec(dllimport)
#	endif
#elif defined(IOL_PLATFORM_LINUX)
#define IOL_API 
#else
#error implement dll stuff for this platform
#endif

#define iol_inline inline
#define iol_bit(x) (1 << (x))
#define iol_countof(arr) sizeof(arr) / sizeof(arr[0])
#define iol_use(x) (void)x

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

#endif // IOLITE_DEFINITIONS_H