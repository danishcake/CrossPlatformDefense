#pragma once

#include "../Component.h"
#include <GLES2/gl2.h>
#include "../../vmath.h"
#include "Effect.h"
#include "Texture.h"

class TexturedQuad : public Component
{
private:
   EffectPtr mEffect;
   TexturePtr mTexture;

   GLuint mVBO;
   int mVBOsize;

   GLint mVertAttributeHandle;
   GLint mTexcAttributeHandle;
public:
   TexturedQuad();
   TexturedQuad(TexturePtr texture);

   virtual void InitialiseGraphics(TickParameters& tp);
   virtual void Tick(TickParameters& tp);
   virtual void Draw(TickParameters& tp);
   virtual void Teardown(TickParameters& tp) {};
   virtual void TeardownGraphics(TickParameters& tp);
};
