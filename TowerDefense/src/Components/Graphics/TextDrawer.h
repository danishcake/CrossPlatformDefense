#pragma once
#include "../Component.h"
#include <GLES2/gl2.h>
#include "Effect.h"
#include "../Graphics/Font.h"
#include "../../vmath.h"

/*
 * Text drawer
 * Generates a VBO packing a string in screenspace
 */
class TextDrawer : public Component
{
private:
   std::string mText;
   GLuint mVBO;
   int mVBOsize;
   FontPtr mFont;
   EffectPtr mEffect;
   Matrix4f mTransform;

   GLint mPositionAttributeHandle;
   GLint mColorAttributeHandle;
   GLint mTransformUniformHandle;
   GLint mTextureCoordinateAttributeHandle;

   float mWidth;
   float mHeight;

public:
   TextDrawer(std::string font, std::string text);
   TextDrawer(std::string font, std::string text, Matrix4f transform);

   virtual void InitialiseGraphics(TickParameters& tp);
   virtual void Tick(TickParameters& tp);
   virtual void Draw(TickParameters& tp);
   virtual void Teardown(TickParameters& tp) {};
   virtual void TeardownGraphics(TickParameters& tp);
};
