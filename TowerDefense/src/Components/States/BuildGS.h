#pragma once

#include "../Component.h"
#include "SharedState.h"
#include "../CursorEventReceiver.h"

class GameObject;
class WorldBlocks;
class ControlProgress;

class BuildGS : public Component
{
private:
   bool mFirst;
   void SpawnMenuObjects(TickParameters& tp);
   void TransitionToDefend(int x, int y, TickParameters& tp);
   void CursorActionPerformed(CursorAction::Enum, TickParameters& tp);

   bool mTransitioningToDefend;
   float mTransitionTimer;

   WorldBlocks* mBlocks;
   SharedState mSharedState;
   ControlProgress* mAddProgress;
   ControlProgress* mRemoveProgress;
   CursorEventReceiver* mCursorEventRcvr;

public:
   BuildGS();
   BuildGS(WorldBlocks* world, SharedState shared_state);
   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp);
};
