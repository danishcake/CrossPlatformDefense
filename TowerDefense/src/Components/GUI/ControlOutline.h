#pragma once

#include "../Component.h"
#include <GLES2/gl2.h>
#include "../../vmath.h"
#include "../Graphics/Effect.h"

class ControlArea;
class ControlEventReceiver;

class ControlOutline : public Component
{
private:
   GLuint mFillVBO;
   int mFillVBOsize;
   GLuint mOutlineVBO;
   int mOutlineVBOsize;

   EffectPtr mEffect;

   ControlArea* mControlArea;
   ControlEventReceiver* mEventRcvr;

   Matrix4f mTransform;
   float mAlpha;

   GLint mPositionAttributeHandle;
   GLint mColorAttributeHandle;
   GLint mTransformUniformHandle;
   GLint mAlphaUniformHandle;

public:
   ControlOutline();

   virtual void Initialise(TickParameters& tp, GameObject* owner);
   virtual void InitialiseGraphics(TickParameters& tp);
   virtual void Tick(TickParameters& tp);
   virtual void Draw(TickParameters& tp);
   virtual void Teardown(TickParameters& tp) {};
   virtual void TeardownGraphics(TickParameters& tp);

   void SetTransform(Matrix4f transform);
   void SetAlpha(float a);
};
