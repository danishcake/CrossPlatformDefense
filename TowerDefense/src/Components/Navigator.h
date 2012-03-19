#pragma once

#include "Component.h"
#include "../vmath.h"
#include "../InvalidationWatcher.h"
class Position;
class PotentialMap;
class WorldBlocks;
class WorldHandle;

class Navigator : public Component
{
private:
   PotentialMap* mPMap;
   Position* mPosition;
   Vector3i mOrigin;
   Vector3i mDelta;
   float mTimer;
   float mTimestep;
   WorldHandle* mWorldHdl;
   InvalidationWatcher mInvalidationWatcher;
public:
   Navigator();

   virtual void Initialise(TickParameters& tp, GameObject* owner);
   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp);
};