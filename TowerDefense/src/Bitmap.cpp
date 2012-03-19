#include "Bitmap.h"
#include "TickParameters.h"
#include "Log.h"

extern "C"
{
#define PNG_NO_PEDANTIC_WARNINGS
#ifdef ANDROID
#include "png_141.h"
#else
#include <png.h>
#endif
}

struct png_read_data
{
   const char* data;
   unsigned int read_cnt;
};

static void ReadDataFromInputStream(png_structp png_ptr, png_bytep outBytes,
   png_size_t byteCountToRead)
{
   png_voidp io_ptr;
   io_ptr = png_get_io_ptr(png_ptr);
   png_read_data* rd_data = (png_read_data*)io_ptr;

   memcpy(outBytes, rd_data->data + rd_data->read_cnt, byteCountToRead);
   rd_data->read_cnt += byteCountToRead;
}

Bitmap::Bitmap()
 : mWidth(0), mHeight(0), mFormat(BitmapFormat::Invalid),
   mLoadState(BitmapLoadState::NotLoaded)
{
}

Bitmap::~Bitmap()
{
   if(mData.use_count() == 1)
   {
      delete[] mData.raw_pointer();
   }
}

Bitmap Bitmap::LoadPNG(TickParameters& tp, std::string filename, bool flip)
{
   Bitmap bmp;

   bmp.mFilename = filename;

   //Load PNG
   //Get pointer to data
   void* fh = tp.file->Open(filename.c_str());
   if(!fh)
   {
      Log::Error("Bitmap::LoadPNG", "Unable to open asset %s", filename.c_str());
      bmp.mLoadState = BitmapLoadState::FileNotfound;
      return bmp;
   }

   png_structp png_ptr;
   png_infop info_ptr;
   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

   if (png_ptr == NULL)
   {
      Log::Error("Bitmap::LoadPNG", "Null from png_create_read_struct");
      tp.file->Close(fh);
      bmp.mLoadState = BitmapLoadState::DecodeError;
      return bmp;
   }
   
  /* Allocate/initialize the memory
   * for image information.  REQUIRED. */
   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL) {
      Log::Error("Bitmap::LoadPNG", "Null from png_create_info_struct");
      tp.file->Close(fh);
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      bmp.mLoadState = BitmapLoadState::DecodeError;
      return bmp;
   }
   
   int bytes_read = 0;
   png_read_data rd_data;
   rd_data.data = (char*)tp.file->ReadAll(fh, &bytes_read);
   rd_data.read_cnt = 0;

   //Set read function
   png_set_read_fn(png_ptr, (void*)&rd_data, ReadDataFromInputStream);

   if (setjmp(png_jmpbuf(png_ptr)))
   {
      /* Free all of the memory associated
       * with the png_ptr and info_ptr */
      Log::Error("Bitmap::LoadPNG", "Error, longjmp save.");
      tp.file->Close(fh);
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      /* If we get here, we had a
       * problem reading the file */
      bmp.mLoadState = BitmapLoadState::DecodeError;
      return bmp;
   }

   //Get png header data
   png_read_info(png_ptr, info_ptr);

   png_uint_32 width = 0;
   png_uint_32 height = 0;
   int bitDepth = 0;
   int colorType = -1;
   png_uint_32 retval = png_get_IHDR(png_ptr, info_ptr,
      &width,
      &height,
      &bitDepth,
      &colorType,
      NULL, NULL, NULL);

   //Determine bpp
   bool hasAlpha = false;
   switch (colorType)
   {
      case PNG_COLOR_TYPE_RGBA:
         hasAlpha = true;
         bmp.mFormat = BitmapFormat::RGBA;
         break;
      case PNG_COLOR_TYPE_RGB:
         hasAlpha = false;
         bmp.mFormat = BitmapFormat::RGB;
         break;
      default:
         Log::Error("WorldDrawer", "PNG unsupported color type %d.", colorType);
         tp.file->Close(fh);
         png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
         bmp.mLoadState = BitmapLoadState::DecodeError;
         return bmp;
   }
   unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);

   //Allocate whole image buffer and copy rows into it
   bmp.mWidth = width;
   bmp.mHeight = height;
   bmp.mData = RefCnt<char>(new char[row_bytes * height]);
   if(!bmp.mData.raw_pointer())
   {
      Log::Error("Bitmap::LoadPNG", "Unable to allocate memory: %d bytes.", row_bytes * height);
      tp.file->Close(fh);
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      bmp.mLoadState = BitmapLoadState::DecodeError;
      return bmp;
   }
   
   
   png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

   char* rowData = new char[row_bytes];
   for (unsigned int i = 0; i < height; i++)
   {
      png_read_row(png_ptr, (png_bytep)rowData, NULL);
      // note that png is ordered top to
      // bottom, but OpenGL expect it bottom to top
      // so the order or swapped
      if(flip)
         memcpy(bmp.mData.raw_pointer() + (row_bytes * (height-1-i)), rowData, row_bytes);
      else
         memcpy(bmp.mData.raw_pointer() + row_bytes * i, rowData, row_bytes);
   }
   delete[] rowData;

   png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
   //Close file now we're done with it.
   tp.file->Close(fh);
   
   
   bmp.mLoadState = BitmapLoadState::OK;
   return bmp;
}

Pixel Bitmap::GetPixel(const int x, const int y) const
{
   int xc = x;
   int yc = y;
   xc = xc < 0 ? 0 : xc >= mWidth ? mWidth - 1 : xc;
   yc = yc < 0 ? 0 : yc >= mWidth ? mWidth - 1 : yc;

   Pixel rgba;
   if(mFormat != BitmapFormat::Invalid && mLoadState == BitmapLoadState::OK)
   {
      rgba = *(Pixel*)(mData.raw_pointer() + (yc * mWidth  + xc) * (int)mFormat);
   }
   
   return rgba;
}
