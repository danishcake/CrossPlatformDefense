#pragma once

#include "../Component.h"
#include <GLES2/gl2.h>
#include "../../vmath.h"
#include "Effect.h"

namespace FadeType
{
   enum Enum
   {
      FadeIn, FadeOut, FadeOutThenIn
   };
}

/*
 * FadeOverlay draws a quad over the whole screen. This is faded between 
 * transparent and a provided color
 */
class FadeOverlay : public Component
{
private:
   EffectPtr mEffect;

   GLuint mVBO;
   int mVBOsize;
   Vector3f mFadeColor;
   float mFadeTime;
   float mFadeTimer;
   float mFadeScalar;
   FadeType::Enum mFadeType;

   GLint mPositionAttributeHandle;
   GLint mFadeScalarUniformHandle;
   GLint mFadeColorUniformHandle;
public:
   FadeOverlay(FadeType::Enum fade_type, Vector3f fade_color, float fade_time);

   virtual void InitialiseGraphics(TickParameters& tp);
   virtual void Tick(TickParameters& tp);
   virtual void Draw(TickParameters& tp);
   virtual void Teardown(TickParameters& tp) {};
   virtual void TeardownGraphics(TickParameters& tp);
};
