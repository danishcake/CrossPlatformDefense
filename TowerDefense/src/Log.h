#pragma once

#include <cstdio>
#include <cstdarg>
#include <cstring>

namespace Log
{
   extern void Debug(const char* loc, const char* fmt, ...);
   extern void Error(const char* loc, const char* fmt, ...);
}
