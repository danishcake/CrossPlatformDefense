#pragma once
#include <map>
#include <string>
#include "Texture.h"
#include "../../RefCnt.h"

class Font;
struct TickParameters;

typedef RefCnt<Font> FontPtr;

namespace FontClearLevel
{
   enum Enum
   {
      None, ClearUnused, ClearAll, Teardown, Initialise
   };
}


class Font
{
private:
   unsigned char mWidths[256];
   float         mAdvances[256];
   unsigned char mKerns[256*256];
   char          mYOffset[256];
   std::string   mFilename;
   int           mSizePt;
   bool          mInitialised;
   TexturePtr    mTexture;


   static std::map<std::string, FontPtr> sCache;
   Font(std::string filename);

public:
   static FontPtr GetFont(std::string filename);
   static void CacheTick(TickParameters& tp, FontClearLevel::Enum clear_level);

   TexturePtr GetTexture() { return mTexture; }
   virtual void InitialiseGraphics(TickParameters& tp);
   virtual void TeardownGraphics(TickParameters& tp);

   const unsigned char GetWidth(const char c) const { return mWidths[c]; }
   const float GetAdvance(const char c) const { return mAdvances[c]; }
   const unsigned char GetKern(const char c1, const char c2) const { return mKerns[c1 * 255 + c2]; }
   const char GetYOffset(const char c) const { return mYOffset[c]; }
};
