#pragma once
#include "IFile.h"

#include <map>

struct FileRecord
{
   char* data;
   long len;
};


/*
 * Implements platform independent basic file IO
 * Intention is to build a standard c++ thin wrapper version too
 * but I'll get around to that later (or probably never)
 */
class PlaybookFile : public IFile
{
private:
   std::map<void*, FileRecord> mBuffers;
public:
   PlaybookFile();
   virtual ~PlaybookFile();

   virtual void* Open(const char* const filename);
   virtual void Close(void* handle);
   //Reads size bytes from specified file handle. Returns negative in case of error, less than size if EOF reached.
   virtual int Read(void* buf, std::size_t size, void* handle);
   //Returns a buffer with entire file in it. Needs to be freed by a Close afterwards
   //Size of data written to size
   virtual const void* ReadAll(void* handle, int* size);
};
