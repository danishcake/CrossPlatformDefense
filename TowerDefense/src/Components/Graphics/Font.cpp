#include "Font.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftglyph.h>
#include "../../Log.h"
#include "../../TickParameters.h"

std::map<std::string, FontPtr> Font::sCache;

FontPtr Font::GetFont(std::string font_file)
{
   std::map<std::string, FontPtr>::iterator it = sCache.find(font_file);
   if(it != sCache.end())
   {
      return it->second;
   } else
   {
      FontPtr font_ptr = FontPtr(new Font(font_file));
      sCache[font_file] = font_ptr;
      return font_ptr;
   }
}

void Font::CacheTick(TickParameters& tp, FontClearLevel::Enum clear_level)
{
   if(clear_level == FontClearLevel::None)
      return;

   //Teardown fonts if Teardown passed in
   if(clear_level == FontClearLevel::Teardown)
   {
      for(std::map<std::string, FontPtr>::iterator it = sCache.begin(); it != sCache.end(); ++it)
      {
         it->second->TeardownGraphics(tp);
      }
      return;
   }

   //Initialise fonts if Initialise passed in
   if(clear_level == FontClearLevel::Initialise)
   {
      for(std::map<std::string, FontPtr>::iterator it = sCache.begin(); it != sCache.end(); ++it)
      {
         it->second->InitialiseGraphics(tp);
      }
      return;
   }

   //Clear unused fonts if ClearUnused passed in
   for(std::map<std::string, FontPtr>::iterator it = sCache.begin(); it != sCache.end(); ++it)
   {
      if(clear_level == FontClearLevel::ClearAll ||
         (clear_level == FontClearLevel::ClearUnused && it->second.use_count() == 1))
      {
         it->second->TeardownGraphics(tp);
      }
   }
   std::map<std::string, FontPtr>::iterator it = sCache.begin();
   while(it != sCache.end())
   {
      if(it->second.use_count() == 1)
      {
         sCache.erase(it++);
      } else
      {
         ++it;
      }
   }
}

//Freetype related forward declarations
static FT_Error determine_size(FT_Face face, int* size);
static FT_Error render_lut(FT_Face face, int font_size,
                           unsigned char** bmp_out, unsigned char* widths, float* advances,
                           unsigned char* kerns, char* y_offset);

Font::Font(std::string filename)
   : mFilename(filename), mInitialised(false)
{
}


void Font::InitialiseGraphics(TickParameters& tp)
{
   if(mInitialised)
      return;

   FT_Library library;
   FT_Error err = FT_Init_FreeType(&library);
   if(err != 0)
   {
      Log::Error("Font.cpp", "Error initialising FreeType");
      FT_Done_FreeType(library);
      return;
   }

   FT_Face face;
   void* file = tp.file->Open(mFilename.c_str());
   int file_size;
   const void* file_data = tp.file->ReadAll(file, &file_size);

   err = FT_New_Memory_Face(library, (const FT_Byte*)file_data, file_size, 0, &face);
   if(err == FT_Err_Unknown_File_Format)
   {
      Log::Error("Font.cpp", "Error loading font, unknown format");
      FT_Done_FreeType(library);
      tp.file->Close(file);
      return;
   } else if(err != 0)
   {
      Log::Error("Font.cpp", "Generic error loading font");
      FT_Done_FreeType(library);
      tp.file->Close(file);
      return;
   }

   unsigned char* bmp = 0;

   mSizePt = 16;
   err = determine_size(face, &mSizePt);
   if(err)
   {
      tp.file->Close(file);
      return;
   }
   err = render_lut(face, mSizePt, &bmp, mWidths, mAdvances, mKerns, mYOffset);

   //Free file
   tp.file->Close(file);

   mTexture = TexturePtr(new Texture(bmp, 256, 256, 1));

   //Free bitmaps
   delete[] bmp;

   FT_Done_FreeType(library);
   mInitialised = true;
}

void Font::TeardownGraphics(TickParameters& tp)
{
   if(mInitialised)
   {
      mTexture->TeardownGraphics(tp);
      delete mTexture.raw_pointer();
      mTexture = TexturePtr();
   }
   mInitialised = false;
}





static FT_Error determine_size(FT_Face face, int* size)
{
   //Finds the largest size that allows each character to fit within size*size box.
   //Size is then populated with this font size
   int max_dimension = *size - 2;
   char text[256];
   for(int i = 0; i < 256; i++)
   {
      text[i] = (char)i;
   }

   //Assume we start small enough
   for(int s = 2; s < 128; s++)
   {
      FT_Error err = FT_Set_Char_Size(face, 0, s * 64, 72, 72);
      if(err != 0)
      {
         Log::Error("Font.cpp", "Error setting font size");
         return err;
      }

      for(int c = 0; c < 256; c++)
      {
         FT_UInt glyph_index = FT_Get_Char_Index(face, text[c]);

         FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);

         FT_Glyph glyph;
         FT_Get_Glyph(face->glyph, &glyph);

         FT_BBox bb;
         FT_Glyph_Get_CBox(glyph, ft_glyph_bbox_pixels, &bb);

         if(bb.xMax - bb.xMin > max_dimension || bb.yMax - bb.yMin > max_dimension)
         {
            *size = s - 1;
            return 0;
         }
      }
   }
   *size = 31;
   return 0;
}


static FT_Error render_lut(FT_Face face, int font_size,
                           unsigned char** bmp_out, unsigned char* widths, float* advances,
                           unsigned char* kerns, char* y_offset)
{
   //Allocate bitmap (8bpp), character widths and kern lengths
   unsigned char* bmp      = new unsigned char[256 * 256];

   memset(bmp, 0, 256 * 256);

   //For each character render the glyph and copy to buffer
   //Also store kerning data
   FT_Error err = FT_Set_Char_Size(face, 0, font_size * 64, 72, 72);
   for(int i = 0; i < 256; i++)
   {
      FT_Long glyph_index = FT_Get_Char_Index(face, (char)i);

      err = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
      if(err != 0)
      {
         Log::Error("Font.cpp", "Error loading glyph\n");
      }

      FT_GlyphSlot slot = face->glyph;
      err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
      if(err != 0)
      {
         Log::Error("Font.cpp", "Error rendering glyph\n");
      }

      //Get size to determine how far to offset
      FT_Glyph glyph;
      FT_Get_Glyph(face->glyph, &glyph);
      FT_BBox bb;
      FT_Glyph_Get_CBox(glyph, ft_glyph_bbox_pixels, &bb);
      y_offset[i] = (char)bb.yMax;

      //Copy bitmap, accounting for supersampling
      int x = i % 16;
      int y = i / 16;
      int h_bearing = static_cast<int>(slot->metrics.horiBearingX / 64.0f);
      for(int y2 = 0; y2 < slot->bitmap.rows; y2++)
      {
         for(int x2 = 0; x2 < slot->bitmap.width; x2++)
         {
            int ox = x * (16) + h_bearing + x2;
            int oy = y * (16) + y2;
            if(x2 + h_bearing >= 16 || y2 >= 16)
            {
               Log::Debug(__FILE__, "UH OH!");
            }


            bmp[oy * 256 + ox] = slot->bitmap.buffer[y2 * slot->bitmap.pitch + x2];
         }
      }
      //Copy glyph width and advance
      widths[i] = slot->bitmap.width;
      advances[i] = slot->linearHoriAdvance / (65536.0f);

      if(advances[i] < widths[i] + 1) advances[i] = static_cast<float>(widths[i] + 1);

      //if(i > 31 && i < 127) Log::Debug("Font.cpp", "Character %c: Width:%d Advance:%f", i, widths[i], advances[i]);
      //Copy kerning data
      for(int j = 0; j < 256; j++)
      {
         FT_Long previous_glyph_index = FT_Get_Char_Index(face, (char)j);
         if(FT_HAS_KERNING(face))
         {
            FT_Vector delta;
            FT_Get_Kerning(face, previous_glyph_index, glyph_index, FT_KERNING_DEFAULT, &delta);
            kerns[i * 256 + j] = (unsigned char)(delta.x >> 6);
         } else
         {
            kerns[i * 256 + j] = 0;
         }
         /*
         if(i > 31 && i < 127 && j > 31 && j < 127 && kerns[i * 256 + j] > 0)
            Log::Debug("Font.cpp", "%c kern to %c:%d", (char)i, (char)j, kerns[i * 256 + j]);
         */
      }


      FT_Done_Glyph(glyph);
   }

   *bmp_out = bmp;
   return 0;
}
