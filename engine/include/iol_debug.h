#ifndef IOLITE_DEBUG_H
#define IOLITE_DEBUG_H

#include "iol_definitions.h"

#ifdef IOL_PLATFORM_LINUX
#include <signal.h>
#endif

namespace iol
{
	IOL_API void debug_output_string(const char* pFormat, ...);
	IOL_API void debug_output_string_to_log_file(const char* pFormat, ...);
}

#if defined(IOL_DEBUG)

#ifdef IOL_PLATFORM_WINDOWS
#define iol_breakpoint __debugbreak()
#elif defined(IOL_PLATFORM_LINUX)
#define iol_breakpoint raise(SIGTRAP)
#else
#error unknown configuration
#endif

#define iol_log(format, ...)							iol::debug_output_string("[debug] "); iol::debug_output_string(format, ##__VA_ARGS__); iol::debug_output_string("\n")
#define iol_log_warning(format, ...)					iol::debug_output_string("[warning] "); iol::debug_output_string(format, ##__VA_ARGS__); iol::debug_output_string("\n")
#define iol_log_error(format, ...)						iol::debug_output_string("[error] "); iol::debug_output_string(format, ##__VA_ARGS__); iol::debug_output_string("\n")
#define iol_log_raw(format, ...)						iol::debug_output_string(format, ##__VA_ARGS__); iol::debug_output_string("\n")
#define iol_log_raw_warning(format, ...)				iol::debug_output_string(format, ##__VA_ARGS__); iol::debug_output_string("\n")
#define iol_log_raw_error(format, ...)					iol::debug_output_string(format, ##__VA_ARGS__); iol::debug_output_string("\n")
#define iol_assert(condition)							do { if (!(condition)) iol_breakpoint; } while (0)
#define iol_assert_compile_time(condition)				static_assert(condition, "static assertion failed")
#define iol_verify(condition)							do { if (!(condition)) iol_breakpoint; } while (0)

#elif defined(IOL_RELEASE)
#define iol_breakpoint								
#define iol_log(format, ...)							
#define iol_log_warning(format, ...)					iol::debug_output_string_to_log_file("[warning] "); iol::debug_output_string_to_log_file(format, ##__VA_ARGS__); iol::debug_output_string_to_log_file("\n")
#define iol_log_error(format, ...)						iol::debug_output_string_to_log_file("[error] "); iol::debug_output_string_to_log_file(format, ##__VA_ARGS__); iol::debug_output_string_to_log_file("\n")
#define iol_log_raw(format, ...)						
#define iol_log_raw_warning(format, ...)				iol::debug_output_string_to_log_file(format, ##__VA_ARGS__); iol::debug_output_string_to_log_file("\n")
#define iol_log_raw_error(format, ...)					iol::debug_output_string_to_log_file(format, ##__VA_ARGS__); iol::debug_output_string_to_log_file("\n")
#define iol_assert(condition)							
#define iol_assert_compile_time(condition)				static_assert(condition, "static assertion failed")
#define iol_verify(condition)							condition

#elif defined(IOL_MASTER)
#define iol_breakpoint								
#define iol_log(format, ...)							
#define iol_log_warning(format, ...)					
#define iol_log_error(format, ...)						iol::debug_output_string_to_log_file("[error] "); iol::debug_output_string_to_log_file(format, ##__VA_ARGS__); iol::debug_output_string_to_log_file("\n")
#define iol_log_raw(format, ...)						
#define iol_log_raw_warning(format, ...)				
#define iol_log_raw_error(format, ...)					iol::debug_output_string_to_log_file(format, ##__VA_ARGS__); iol::debug_output_string_to_log_file("\n")
#define iol_assert(condition)							
#define iol_assert_compile_time(condition)				static_assert(condition, "static assertion failed")
#define iol_verify(condition)							condition

#else
#error unknown configuration
#endif

#endif // IOLITE_DEBUG_H
