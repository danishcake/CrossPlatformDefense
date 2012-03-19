#include "Log.h"
#include <stdarg.h>

#ifdef ANDROID
#include <android/log.h>
#endif

namespace Log
{
   void Debug(const char* loc, const char* fmt, ...)
   {
      int nSize = 0;
      char buff[512];
      std::memset(buff, 0, sizeof(buff));
      std::va_list args;
      va_start(args, fmt);

#if defined ANDROID || defined _WIN32
      nSize = vsnprintf( buff, sizeof(buff) - 1, fmt, args); // C4996
#endif
#ifdef __QNX__
      nSize = std::vsnprintf( buff, sizeof(buff) - 1, fmt, args); // C4996
#endif

#ifdef ANDROID
      __android_log_print(ANDROID_LOG_DEBUG, loc, buff);
#else
      std::printf("%s\n", buff);
#endif
   }

   void Error(const char* loc, const char* fmt, ...)
   {
      int nSize = 0;
      char buff[512];
      std::memset(buff, 0, sizeof(buff));
      std::va_list args;
      va_start(args, fmt);

#if defined ANDROID || defined _WIN32
      nSize = vsnprintf( buff, sizeof(buff) - 1, fmt, args); // C4996
#endif
#ifdef __QNX__
      nSize = std::vsnprintf( buff, sizeof(buff) - 1, fmt, args); // C4996
#endif
#ifdef ANDROID
      __android_log_print(ANDROID_LOG_ERROR, loc, buff);
#else
      std::printf("%s\n", buff);
#endif
   }
}
