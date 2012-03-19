#pragma once

#include "../Component.h"
#include <GLES2/gl2.h>
#include "../Graphics/Effect.h"
#include "../Graphics/Texture.h"
#include "../../vmath.h"

class ControlArea;

/*
 * ControlText renders a piece of text to a texture at a maximum size
 * defined by the control box
 * TODO: Allow specific pt size (with dpi derived from device)
 */
class ControlText : public Component
{
private:
   GLuint mFillVBO;
   int mFillVBOsize;

   EffectPtr mEffect;
   TexturePtr mTexture;

   std::string mFont;
   std::string mText;
   Vector4f mColor;
   Matrix4f mTransform;

   ControlArea* mControlArea;

   GLint mPositionAttributeHandle;
   GLint mTxCoordAttributeHandle;
   GLint mTransformUniformHandle;
   GLint mColorUniformHandle;

public:
   ControlText(std::string text, std::string font, Vector4f color);

   virtual void Initialise(TickParameters& tp, GameObject* owner);
   virtual void InitialiseGraphics(TickParameters& tp);
   virtual void Tick(TickParameters& tp);
   virtual void Draw(TickParameters& tp);
   virtual void Teardown(TickParameters& tp) {};
   virtual void TeardownGraphics(TickParameters& tp);

   void SetTransform(Matrix4f transform);
   void SetAlpha(float a);
};
