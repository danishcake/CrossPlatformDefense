#include "AndroidFile.h"

#include <cstring>
#include <sys/types.h>
#include <android/asset_manager.h>

AndroidFile::AndroidFile(AAssetManager* mgr)
: mMgr(mgr)
{
}

void* AndroidFile::Open(const char* const filename)
{
	return (void*)AAssetManager_open(mMgr, filename, AASSET_MODE_RANDOM);
}

void AndroidFile::Close(void* handle)
{
	AAsset_close((AAsset*)handle);
}

int AndroidFile::Read(void* buf, size_t size, void* handle)
{
	return AAsset_read((AAsset*)handle, buf, size);
}

const void* AndroidFile::ReadAll(void* handle, int* size)
{
	*size = (int)AAsset_getLength((AAsset*)handle);
	return AAsset_getBuffer((AAsset*)handle);
}
