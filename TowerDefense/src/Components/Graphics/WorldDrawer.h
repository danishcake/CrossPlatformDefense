#pragma once

#include "../../CommonDefs.h"
#include "../Component.h"
#include <GLES2/gl2.h>
#include "../../vmath.h"
#include "Effect.h"
#include "Texture.h"
#include "../../InvalidationWatcher.h"

class WorldBlocks;

#define COLUMN_SIZE 4

class WorldDrawer : public Component
{
private:
   GLuint mVBO[WORLD_WIDTH * WORLD_BREADTH / (COLUMN_SIZE * COLUMN_SIZE)];
   int mVBOsize[WORLD_WIDTH * WORLD_BREADTH / (COLUMN_SIZE * COLUMN_SIZE)];

   TexturePtr mTexture;
   EffectPtr mEffect;

   GLint mPositionAttributeHandle;
   GLint mTexcAttributeHandle;
   GLint mTransformUniformHandle;

   WorldBlocks* mWorldBlocks;
   InvalidationWatcher mInvalidationWatcher;

   void buildColumn(int x, int z, int vbo_index);

public:
   WorldDrawer(WorldBlocks* blocks);

   virtual void InitialiseGraphics(TickParameters& tp);
   virtual void Tick(TickParameters& tp);
   virtual void Draw(TickParameters& tp);
   virtual void Teardown(TickParameters& tp) {};
   virtual void TeardownGraphics(TickParameters& tp);
};
