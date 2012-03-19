#pragma once

#include <string>
#include "RefCnt.h"
struct TickParameters;

namespace BitmapLoadState
{
   enum Enum
   {
      OK = 0, NotLoaded, FileNotfound, DecodeError
   };
}

namespace BitmapFormat
{
   enum Enum
   {
      Invalid=0, RGB=3, RGBA=4
   };
}

struct Pixel
{
   unsigned char r;
   unsigned char g;
   unsigned char b;
   unsigned char a;
};

/* Bitmap
 * Contains a decoded bitmap that is either 24bpp or 32bpp
 * Created from PNGs
 * Cleans up memory on destruction
 * Not copyable
 */
class Bitmap
{
private:
   int mWidth;
   int mHeight;
   RefCnt<char> mData;
   BitmapFormat::Enum mFormat;
   BitmapLoadState::Enum mLoadState;
   std::string mFilename;

   void renderLUT();

public:
   static Bitmap LoadPNG(TickParameters& tp, std::string filename, bool flip);
   Bitmap();
   ~Bitmap();

   int GetWidth() const {return mWidth;}
   int GetHeight() const {return mHeight;}
   char* GetData() const {return mData.raw_pointer();}
   Pixel GetPixel(const int x, const int y) const;
   BitmapFormat::Enum GetFormat() const {return mFormat;}
   BitmapLoadState::Enum GetLoadState() const {return mLoadState;}
   std::string GetFilename() const {return mFilename;}
};
