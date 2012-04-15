#pragma once

#include "../Component.h"
#include "SharedState.h"
#include "../../Messaging/MessageHub.h"
#include "../../Messaging/WalkerDeathMessage.h"

class GameObject;
class WorldBlocks;
class ControlText;

/*
 * DefendGS is created by BuildGS. It spawns walkers at the map edge who 
 * then navigate to the centre.
 * Tapping a walker kills it.
 * Built towers kill walkers.
 * Walkers spawn a block on death. It becomes solid after a few seconds
 * Tapping the block before it becomes solid picks it up (+n to blocks)
 *
 * The overall gamestate that is shared between the build and defend stage
 * eg block stock, score, wave number 
 * is passed to the constructor
 * This is used to determine the contents of the next wave
 * After all waves on a level are complete it will hand over to a 'StartNextLevel' GS
 *
 */
class DefendGS : public Component
{
private:
   bool mFirst;
   WorldBlocks* mBlocks;
   SharedState mSharedState;
   Subscriber<WalkerDeathMessage> mDeathSubscriber;

   int mSpawnCount;
   int mMaxSpawn;
   int mDeathCount;
   float mSpawnTime;

   bool mTransitioningToBuild;
   float mTransitionTimer;

   ControlText* mWalkerCountText;

   void SpawnObjects(TickParameters& tp);
   void SpawnWalker(TickParameters& tp);
   void TransitionToBuild(TickParameters& tp);
   void TapToKill(int x, int y, TickParameters& tp);
   void UpdateWalkerText(TickParameters& tp);


public:
   DefendGS(WorldBlocks* world, SharedState state);
   virtual void Initialise(TickParameters& tp, GameObject* owner);
   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp);
};
