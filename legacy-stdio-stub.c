// Legacy stdio stub for MinGW zlib compatibility with MSVC
#include <stdio.h>
#include <stdarg.h>

// Provide the legacy __ms_vsnprintf symbol that MinGW zlib expects
int __ms_vsnprintf(char *buffer, size_t count, const char *format, va_list argptr)
{
    return vsnprintf(buffer, count, format, argptr);
}
