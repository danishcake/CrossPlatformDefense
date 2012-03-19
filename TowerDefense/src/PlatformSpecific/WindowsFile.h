#pragma once
#include "IFile.h"
#include <map>

struct FileRecord
{
   char* data;
   long len;
};

/*
 * Implements ansi C version platform independent basic file IO
 * Will need to retain a list of open files so that I can keep track of ReadAll buffers
 */
class WindowsFile : public IFile
{
private:
   std::map<void*, FileRecord> mBuffers;
public:
   WindowsFile();
   ~WindowsFile();

   virtual void* Open(const char* const filename);
   virtual void Close(void* handle);
   //Reads size bytes from specified file handle. Returns negative in case of error, less than size if EOF reached.
   virtual int Read(void* buf, size_t size, void* handle);
   //Returns a buffer with entire file in it. Needs to be freed by a Close afterwards
   //Size of data written to size
   virtual const void* ReadAll(void* handle, int* size);
};
