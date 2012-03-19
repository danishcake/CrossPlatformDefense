#pragma once
#include <cstring>

class IFile
{
public:
   virtual void* Open(const char* const filename) = 0;
   virtual void Close(void* handle) = 0;
   virtual int Read(void* buf, std::size_t size, void* handle) = 0;
   virtual const void* ReadAll(void* handle, int* size) = 0;
};
