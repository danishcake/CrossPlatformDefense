#pragma once

#include "../Component.h"
#include "SharedState.h"

class GameObject;
class WorldBlocks;

class BuildGS : public Component
{
private:
   bool mFirst;
   void SpawnMenuObjects(TickParameters& tp);
   void TransitionToDefend(int x, int y, TickParameters& tp);

   bool mTransitioningToDefend;
   float mTransitionTimer;
   
   WorldBlocks* mBlocks;
   SharedState mSharedState;

public:
   BuildGS();
   BuildGS(WorldBlocks* world, SharedState shared_state);
   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp);
};
