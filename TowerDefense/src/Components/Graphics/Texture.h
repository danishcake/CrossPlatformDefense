#pragma once

#include <GLES2/gl2.h>
#include <map>
#include <string>
#include "../../RefCnt.h"
#include "../GUI/UDim.h"

class Texture;
struct TickParameters;

typedef RefCnt<Texture> TexturePtr;

namespace TextureClearLevel
{
   enum Enum
   {
      None, ClearUnused, ClearAll, Teardown, Initialise
   };
}

struct TextDefinition
{
   std::string mText;
   std::string mFont;
   UDim mSize;

   Vector2f mTX; //Texture coordinates of bottom right corner
   Vector2f mMeasurement; //Size in pixels of text

   TextDefinition(std::string text, std::string font, UDim size)
      : mText(text), mFont(font), mSize(size)
   {
   }
};

struct TextDefinitionComparer
{
   /*
    * Map requires ordering so that this operator when applied will be return
    * where A != B
    * (A, B) => true
    * (B, A) => false,
    * eg (A, B) = !(B, A)
    * Also if A==B then return false
    */
   bool operator()(const TextDefinition& left, const TextDefinition& right) const
   {
      if (left.mFont == right.mFont && left.mText == right.mText && left.mSize == right.mSize)
      {
         return false;
      }
      bool left_less = (left.mFont < right.mFont ||
                        left.mText < right.mText ||
                        !(left.mSize == right.mSize));
      return !left_less;
   }
};


/*
 * Texture encapsulates the loading of .png textures
 * to OpenGL.
 * It also will render an ASCII table using FreeType
 */
class Texture
{
private:
   std::string mTextureFile;
   TextDefinition mTextDef;
   GLuint mTexture;
   bool mInitialised;
   //Private, use GetTexture to go via cache
   Texture(std::string texture_file);
   Texture(std::string text, std::string font, UDim size);

   static std::map<std::string, TexturePtr> sCache;
   static std::map<TextDefinition, TexturePtr, TextDefinitionComparer> sCacheText;

   void InitialisePNG(TickParameters& tp);
   void InitialiseText(TickParameters& tp);

public:
   /* GetTexture
    * Loads a .png file
    */
   static TexturePtr GetTexture(std::string texture_file);
   static TexturePtr GetTexture(std::string text, std::string font, UDim size);

   /*
    * Textures created by this route are not managed by the cache and cannot be recreated
    * when invalidated - other code will have to do this (eg Font)
    */
   Texture(unsigned char* data, int w, int h, int bpp);
   

   static void CacheTick(TickParameters& tp, TextureClearLevel::Enum clear_level);

   virtual void InitialiseGraphics(TickParameters& tp);
   virtual void TeardownGraphics(TickParameters& tp);
   void UseTexture(int slot);
   void StopUsingTexture();

   const TextDefinition GetText() { return mTextDef;}
};
