#include "WindowsFile.h"

#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include "../Log.h"

WindowsFile::WindowsFile()
{
}

WindowsFile::~WindowsFile()
{
   std::map<void*, FileRecord> map_copy = mBuffers;

   for(std::map<void*, FileRecord>::iterator it = map_copy.begin(); it != map_copy.end(); ++it)
   {
      Close(it->first);
   }
}

void* WindowsFile::Open(const char* const filename)
{
   FILE* file = fopen(filename, "rb");
   if(!file)
   {
      Log::Error(__FILE__, "Unable to open file %s", filename);
   }
   return (void*)file;
}

void WindowsFile::Close(void* handle)
{
   fclose((FILE*)handle);

   std::map<void*, FileRecord>::iterator buf_it = mBuffers.find(handle);
   if(buf_it != mBuffers.end())
   {
      delete[] buf_it->second.data;
      mBuffers.erase(buf_it);
   }
}


int WindowsFile::Read(void* buf, size_t size, void* handle)
{
   return (int)fread(buf, 1, size, (FILE*)handle);
}

const void* WindowsFile::ReadAll(void* handle, int* size)
{
   std::map<void*, FileRecord>::iterator buf_it = mBuffers.find(handle);
   if(buf_it == mBuffers.end())
   {
      fseek((FILE*)handle, 0, SEEK_END);
      long filesize = ftell((FILE*)handle);
      fseek((FILE*)handle, 0, 0);

      void* buf = (void*)new char[filesize + 1];

      fread(buf, filesize, 1, (FILE*)handle);

      FileRecord fr;
      fr.data = (char*)buf;
      fr.data[filesize] = 0;
      fr.len = filesize;

      mBuffers[handle] = fr;
   }

   *size = mBuffers[handle].len;
   return mBuffers[handle].data;
};
