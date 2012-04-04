#include "Texture.h"
#include "../../Log.h"
#include "../../CheckMacros.h"
#include "../../Bitmap.h"
#include <GLES2/gl2.h>


#include <cstdio>

std::map<std::string, TexturePtr> Texture::sCache;
TexturePtr Texture::GetTexture(std::string texture_file)
{
   std::map<std::string, TexturePtr>::iterator tx_it = sCache.find(texture_file);
   if(tx_it != sCache.end())
   {
      return tx_it->second;
   } else
   {
      TexturePtr tx_ptr = TexturePtr(new Texture(texture_file));
      sCache[texture_file] = tx_ptr;
      return tx_ptr;
   }
}

std::map<TextDefinition, TexturePtr, TextDefinitionComparer> Texture::sCacheText;
TexturePtr Texture::GetTexture(std::string text, std::string font, UDim size)
{
   TextDefinition text_def(text, font, size);

   std::map<TextDefinition, TexturePtr, TextDefinitionComparer>::iterator tx_it = sCacheText.find(text_def);
   if(tx_it != sCacheText.end())
   {
      return tx_it->second;
   } else
   {
      TexturePtr tx_ptr = TexturePtr(new Texture(text, font, size));
      sCacheText[text_def] = tx_ptr;
      return tx_ptr;
   }
}


void Texture::CacheTick(TickParameters& tp, TextureClearLevel::Enum clear_level)
{
   if(clear_level == TextureClearLevel::None)
      return;

   //Teardown effects if Teardown passed in
   if(clear_level == TextureClearLevel::Teardown)
   {
      for(std::map<std::string, TexturePtr>::iterator it = sCache.begin(); it != sCache.end(); ++it)
      {
         it->second->TeardownGraphics(tp);
      }

      for(std::map<TextDefinition, TexturePtr, TextDefinitionComparer>::iterator it = sCacheText.begin(); it != sCacheText.end(); ++it)
      {
         it->second->TeardownGraphics(tp);
      }
      return;
   }

   //Initialise effects if Initialise passed in
   if(clear_level == TextureClearLevel::Initialise)
   {
      for(std::map<std::string, TexturePtr>::iterator it = sCache.begin(); it != sCache.end(); ++it)
      {
         it->second->InitialiseGraphics(tp);
      }

      for(std::map<TextDefinition, TexturePtr, TextDefinitionComparer>::iterator it = sCacheText.begin(); it != sCacheText.end(); ++it)
      {
         it->second->InitialiseGraphics(tp);
      }
      return;
   }

   //Clear unused textures if ClearUnused passed in
   for(std::map<std::string, TexturePtr>::iterator it = sCache.begin(); it != sCache.end(); ++it)
   {
      if(clear_level == TextureClearLevel::ClearAll ||
         (clear_level == TextureClearLevel::ClearUnused && it->second.use_count() == 1))
      {
         it->second->TeardownGraphics(tp);
      }
   }

   //Clear unused textures if ClearUnused passed in
   for(std::map<TextDefinition, TexturePtr, TextDefinitionComparer>::iterator it = sCacheText.begin(); it != sCacheText.end(); ++it)
   {
      if(clear_level == TextureClearLevel::ClearAll ||
         (clear_level == TextureClearLevel::ClearUnused && it->second.use_count() == 1))
      {
         it->second->TeardownGraphics(tp);
      }
   }

   std::map<std::string, TexturePtr>::iterator it = sCache.begin();
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

   std::map<TextDefinition, TexturePtr, TextDefinitionComparer>::iterator it_txt = sCacheText.begin();
   while(it_txt != sCacheText.end())
   {
      if(it_txt->second.use_count() == 1)
      {
         sCacheText.erase(it_txt++);
      } else
      {
         ++it_txt;
      }
   }
}


Texture::Texture(std::string texture_file)
   : mTextureFile(texture_file),
     mTextDef("","", UDim()),
     mInitialised(false)
{
}

Texture::Texture(std::string text, std::string font, UDim size)
   : mTextureFile(""),
     mTextDef(text, font, size),
     mInitialised(false)
{
}

Texture::Texture(unsigned char* data, int w, int h, int bpp) //Bytes per pixel
   : mTextDef("","", UDim())
{
   //Create OpenGL texture object
   GL_CHECK(glGenTextures(1, &mTexture));

   GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTexture));
   GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

   GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
   GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
   GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
   GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));

   GLenum format;
   switch(bpp)
   {
   case 1:
      format = GL_LUMINANCE;
      break;
   case 3:
      format = GL_RGB;
      break;
   case 4:
   default:
      format = GL_RGBA;
      break;
   }
   GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, 
                         format, GL_UNSIGNED_BYTE, data));

   GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));

   mInitialised = true;
}

void Texture::InitialiseGraphics(TickParameters& tp)
{
   if(mInitialised)
      return; //Already initialised

   if(mTextureFile.length() > 0)
   {
      InitialisePNG(tp);
   }
   else if(mTextDef.mText.length() > 0)
   {
      InitialiseText(tp);
   }
   else
   {
      //Warn
      Log::Error(__FILE__, "Texture is not a .png or a text texture, cannot initialise it. Constructed wrong?");
   }

   return;
}

void Texture::InitialisePNG(TickParameters& tp)
{
   Bitmap bmp(Bitmap::LoadPNG(tp, mTextureFile, true));
   if(bmp.GetLoadState() != BitmapLoadState::OK)
   {
      Log::Error("Texture::IntialiseGraphics", "Unable to load texture %s", mTextureFile.c_str());
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
   GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, bmp.GetFormat() == BitmapFormat::RGBA ? GL_RGBA : GL_RGB, 
                         bmp.GetWidth(), bmp.GetHeight(), 0, 
                         bmp.GetFormat() == BitmapFormat::RGBA ? GL_RGBA : GL_RGB, 
                         GL_UNSIGNED_BYTE, bmp.GetData()));
   GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));

   mInitialised = true;
}

void Texture::UseTexture(int slot)
{
   glActiveTexture(GL_TEXTURE0 + slot);
   glBindTexture(GL_TEXTURE_2D, mTexture);
}

void Texture::StopUsingTexture()
{
   glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::TeardownGraphics(TickParameters& tp)
{
   glDeleteTextures(1, &mTexture);
   mInitialised = false;
}

