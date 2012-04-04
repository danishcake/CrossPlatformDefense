#pragma once

#include "../Component.h"
#include <GLES2/gl2.h>
#include "Effect.h"
#include "../../PotentialMap.h"
#include "../../InvalidationWatcher.h"

class WorldBlocks;

/*
 * PathVisualiser draws arrows pointing in the direction of the 
 * pathfinding
 */
class PathVisualiser : public Component
{
private:
   GLuint mVBO;
   int mVBOsize;

   EffectPtr mEffect;

   GLint mPositionAttributeHandle;
   GLint mTransformUniformHandle;

   InvalidationWatcher mInvalidationWatcher;
   PotentialMap mPotentialMap;
   WorldBlocks* mWorldBlocks;

   void BuildMap();

public:
   PathVisualiser(WorldBlocks* world_blocks);

   virtual void InitialiseGraphics(TickParameters& tp);
   virtual void Tick(TickParameters& tp);
   virtual void Draw(TickParameters& tp);
   virtual void Teardown(TickParameters& tp) {}
   virtual void TeardownGraphics(TickParameters& tp);
};
