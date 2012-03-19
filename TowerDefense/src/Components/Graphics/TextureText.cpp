#include "Texture.h"
#include "../../TickParameters.h"
#include "../../Camera.h"
#include <GLES2/gl2.h>
#include "../../Log.h"
#include "../../CheckMacros.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftglyph.h>

//http://www.freetype.org/freetype2/docs/tutorial/step2.html
FT_Error measure_string(FT_Face face, std::string text, int size, Vector2i* size_out)
{
   int pos_x = 0;
   bool use_kerning = FT_HAS_KERNING(face) ? true : false;
   FT_UInt prev_glyph_index = 0;

   FT_BBox text_bb;
   text_bb.xMax = 0;
   text_bb.xMin = 0;
   text_bb.yMax = 0;
   text_bb.yMin = 0;
   FT_Error error;

   error = FT_Set_Char_Size(face, 0, size * 64, 72, 72);

   for(unsigned int i = 0; i < text.length(); i++)
   {
      FT_UInt glyph_index = FT_Get_Char_Index(face, text.c_str()[i]);

      if(use_kerning && prev_glyph_index)
      {
         FT_Vector delta;
         FT_Get_Kerning(face, prev_glyph_index, glyph_index, FT_KERNING_DEFAULT, &delta);
         pos_x += delta.x >> 6;
      }
      prev_glyph_index = glyph_index;

      if(error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT) != FT_Err_Ok)
      {
         Log::Error(__FILE__, "Unable to load glyph %d", glyph_index);
         return error;
      }

      FT_Glyph glyph;
      if(error = FT_Get_Glyph(face->glyph, &glyph) != FT_Err_Ok)
      {
         Log::Error(__FILE__, "Unable to get glyph %d", glyph_index);
      }

      FT_BBox bb;
      FT_Glyph_Get_CBox(glyph, ft_glyph_bbox_pixels, &bb);
      bb.xMax += pos_x;
      bb.xMin += pos_x;


      pos_x += glyph->advance.x >> 16;

      //Grow overall bounding box
      if(bb.xMax > text_bb.xMax)
         text_bb.xMax = bb.xMax;
      if(bb.yMax > text_bb.yMax)
         text_bb.yMax = bb.yMax;
      if(bb.xMin < text_bb.xMin)
         text_bb.xMin = bb.xMin;
      if(bb.yMin < text_bb.yMin)
         text_bb.yMin = bb.yMin;

      FT_Done_Glyph(glyph);
   }

   *size_out = Vector2i(text_bb.xMax - text_bb.xMin, text_bb.yMax - text_bb.yMin);
   return FT_Err_Ok;
}

static FT_Error render_string(FT_Face face, std::string text, int size, unsigned char* data, int w, int h, int max_height)
{
   int pos_x = 0;
   bool use_kerning = FT_HAS_KERNING(face) ? true : false;
   FT_UInt prev_glyph_index = 0;
   FT_Error err;

   err = FT_Set_Char_Size(face, 0, size * 64, 72, 72);

   for(unsigned int i = 0; i < text.length(); i++)
   {
      FT_UInt glyph_index = FT_Get_Char_Index(face, text.c_str()[i]);

      err = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
      if(err != 0)
      {
         Log::Error(__FILE__, "Error loading glyph");
         return err;
      }

      FT_GlyphSlot slot = face->glyph;
      err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
      if(err != 0)
      {
         Log::Error(__FILE__, "Error rendering glyph");
         return err;
      }

      //Get size to determine how far to offset
      FT_Glyph glyph;
      if(err = FT_Get_Glyph(face->glyph, &glyph) != FT_Err_Ok)
      {
         Log::Error(__FILE__, "Cannot get glyph %d", glyph_index);
         return err;
      }

      FT_BBox bb;
      FT_Glyph_Get_CBox(glyph, ft_glyph_bbox_pixels, &bb);

      //Copy bitmap
      for(int y = 0; y < slot->bitmap.rows; y++)
      {
         for(int x = 0; x < slot->bitmap.width; x++)
         {
            int ox = pos_x + x;
            int oy = y;//h - 1 - y;
            oy += max_height - bb.yMax;
            data[oy * w + ox] = slot->bitmap.buffer[y * slot->bitmap.pitch + x];
         }
      }

      pos_x += static_cast<int>(slot->linearHoriAdvance / (65536.0f));

      if(FT_HAS_KERNING(face) && prev_glyph_index)
      {
         FT_Vector delta;
         FT_Get_Kerning(face, prev_glyph_index, glyph_index, FT_KERNING_DEFAULT, &delta);
         pos_x += delta.x >> 6;
      }
      prev_glyph_index = glyph_index;

      FT_Done_Glyph(glyph);
   }

   return FT_Err_Ok;
}

void Texture::InitialiseText(TickParameters& tp)
{
   //Calculate size in pixels of texture required

   Vector2f screen_size = mTextDef.mSize.ToScreen(tp.camera->GetResolution());
   //Now indentify the nearest power of 2 texture size greater than this

   GLint max_texture_size;
   glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);

   int w, h;
   for(w = 1; w < 512; w *= 2)
   {
      if(w >= screen_size.x)
         break;
   }

   for(h = 1; h < 512; h *= 2)
   {
      if(h >= screen_size.y)
         break;
   }

   if(w > max_texture_size || h > max_texture_size)
   {
      Log::Error(__FILE__, "TextTexture size %dx%d larger than supported %dx%d. Cannot render %s", 
                 w, h, max_texture_size, max_texture_size, mTextDef.mText);
      return;
   }


   //Render the texture using FreeType
   FT_Library library;
   FT_Error err = FT_Init_FreeType(&library);
   if(err != 0)
   {
      Log::Error(__FILE__, "Error initialising FreeType");
      FT_Done_FreeType(library);
      return;
   }

   FT_Face face;
   void* file = tp.file->Open(mTextDef.mFont.c_str());
   int file_size;
   const void* file_data = tp.file->ReadAll(file, &file_size);

   err = FT_New_Memory_Face(library, (const FT_Byte*)file_data, file_size, 0, &face);


   if(err == FT_Err_Unknown_File_Format)
   {
      Log::Error(__FILE__, "Error loading font, unknown format");
      FT_Done_FreeType(library);
      tp.file->Close(file);
      return;
   } else if(err != 0)
   {
      Log::Error(__FILE__, "Generic error loading font");
      FT_Done_FreeType(library);
      tp.file->Close(file);
      return;
   }


   //Determine the best size for the string to fit into
   //by measuring the string at different sizes
   int font_size = 1;
   int max_height = 1;
   for(int size = 1; size < 64; size++)
   {
      Vector2i text_size;
      if(err = measure_string(face, mTextDef.mText, size, &text_size) != FT_Err_Ok)
      {
         Log::Error(__FILE__, "Error measuring string");
         return;
      }
      if(text_size.x > screen_size.x && text_size.y > screen_size.y)
         break;
      else
      {
         font_size = size;
         max_height = text_size.y;
         mTextDef.mTX.x = (float)text_size.x / (float)w;
         mTextDef.mTX.y = (float)text_size.y / (float)h;
         mTextDef.mMeasurement.x = static_cast<float>(text_size.x);
         mTextDef.mMeasurement.y = static_cast<float>(text_size.y);
      }
   }

   //Now render the string to a block of memory
   unsigned char* data = new unsigned char[w * h];
   memset(data, 0, w * h);
   if(render_string(face, mTextDef.mText, font_size, data, w, h, max_height) != FT_Err_Ok)
   {
      Log::Error(__FILE__, "Unable to render string");
      return;
   }


   //Create OpenGL texture object
   GL_CHECK(glGenTextures(1, &mTexture));

   GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTexture));
   GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

   GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
   GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
   GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
   GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
   GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data));
   GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));

   delete[] data;
   mInitialised = true;
}
