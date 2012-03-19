#include "PlaybookFile.h"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <algorithm>
#include "../Log.h"

PlaybookFile::PlaybookFile()
{
}

PlaybookFile::~PlaybookFile()
{
   std::map<void*, FileRecord> map_copy = mBuffers;

   for(std::map<void*, FileRecord>::iterator it = map_copy.begin(); it != map_copy.end(); ++it)
   {
      Close(it->first);
   }
}

void* PlaybookFile::Open(const char* const filename)
{
   std::string decorated_name = std::string("app/native/assets/") + filename;
   std::FILE* file = std::fopen(decorated_name.c_str(), "rb");
   if(!file)
   {
      Log::Error(__FILE__, "Unable to open file %s", filename);
   }
   return (void*)file;
}

void PlaybookFile::Close(void* handle)
{
   std::fclose((std::FILE*)handle);

   std::map<void*, FileRecord>::iterator buf_it = mBuffers.find(handle);
   if(buf_it != mBuffers.end())
   {
      delete[] buf_it->second.data;
      mBuffers.erase(buf_it);
   }
}

int PlaybookFile::Read(void* buf, std::size_t size, void* handle)
{
   return (int)std::fread(buf, 1, size, (std::FILE*)handle);
}

const void* PlaybookFile::ReadAll(void* handle, int* size)
{
   std::map<void*, FileRecord>::iterator buf_it = mBuffers.find(handle);
   if(buf_it == mBuffers.end())
   {
      std::fseek((std::FILE*)handle, 0, SEEK_END);
      long filesize = std::ftell((std::FILE*)handle);
      std::fseek((std::FILE*)handle, 0, 0);

      void* buf = (void*)new char[filesize + 1];

      std::fread(buf, filesize, 1, (std::FILE*)handle);

      FileRecord fr;
      fr.data = (char*)buf;
      fr.data[filesize] = 0;
      fr.len = filesize;

      mBuffers[handle] = fr;
   }

   *size = mBuffers[handle].len;
   return mBuffers[handle].data;
};
