#pragma once

#include "../Component.h"
#include <GLES2/gl2.h>
#include "../../vmath.h"
#include "../Graphics/Effect.h"

class ControlArea;

class ControlProgress : public Component
{
private:
   GLuint mFillVBO;
   int mFillVBOsize;

   EffectPtr mEffect;

   ControlArea* mControlArea;

   Matrix4f mTransform;
   float mAlpha;
   float mProgress;
   float mFullWidth;

   GLint mTransformUniformHandle;
   GLint mVertexColorLUTUniformHandle;
   GLint mAlphaUniformHandle;
   GLint mWidthUniformHandle;
   GLint mPositionAttributeHandle;
   GLint mColorAttributeHandle;

public:
   ControlProgress();

   virtual void Initialise(TickParameters& tp, GameObject* owner);
   virtual void InitialiseGraphics(TickParameters& tp);
   virtual void Tick(TickParameters& tp);
   virtual void Draw(TickParameters& tp);
   virtual void Teardown(TickParameters& tp) {};
   virtual void TeardownGraphics(TickParameters& tp);

   void SetTransform(Matrix4f transform);
   void SetAlpha(float a);
   void SetProgress(float progress);
};
