#pragma once

#include "../Component.h"
#include <GLES2/gl2.h>
#include "../../vmath.h"
#include "Effect.h"
#include "Texture.h"

class Position;

class CursorDrawer : public Component
{
private:
   GLuint mVBO;
   int mVBOsize;

   EffectPtr mEffect;
   Position* mPosition;

   GLint mPositionAttributeHandle;
   GLint mTransformUniformHandle;


public:
   CursorDrawer();

   virtual void Initialise(TickParameters& tp, GameObject* owner);
   virtual void InitialiseGraphics(TickParameters& tp);
   virtual void Tick(TickParameters& tp);
   virtual void Draw(TickParameters& tp);
   virtual void Teardown(TickParameters& tp) {};
   virtual void TeardownGraphics(TickParameters& tp);
};
