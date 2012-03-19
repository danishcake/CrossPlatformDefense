#include "Navigator.h"
#include "../PotentialMap.h"
#include "../GameObject.h"
#include "Position.h"
#include "WorldHandle.h"
#include "../WorldBlocks.h"
#include <cmath>

Navigator::Navigator()
 : mTimer(0), mTimestep(0.1f)
{
}

void Navigator::Initialise(TickParameters& tp, GameObject* owner)
{
   Component::Initialise(tp, owner);
   mPosition =  owner->GetComponent<Position>();
   mWorldHdl = owner->GetComponent<WorldHandle>();


   mPMap = new PotentialMap(mWorldHdl->GetWorldBlocks(), 1, 1);
   mPosition->SetPosition(Vector3f(0, static_cast<float>(mWorldHdl->GetWorldBlocks()->getColumn(0, 0).GetHeight() + 1), 0));
   mOrigin = mPosition->GetPosition();
   mInvalidationWatcher.CheckInvalidation(*mWorldHdl->GetWorldBlocks());
}


void Navigator::Tick(TickParameters& tp)
{
   if(mInvalidationWatcher.CheckInvalidation(*mWorldHdl->GetWorldBlocks()))
   {
      mPMap->Evaluate();
   }

   mTimer += tp.timespan;
   if(mTimer > mTimestep)
   {
      mOrigin += mDelta;
      mPMap->Guide(mOrigin.x, mOrigin.z, mDelta.x, mDelta.z);
      mDelta.y = 0;

      //Fall down, step up
      float height_now = static_cast<float>(mWorldHdl->GetWorldBlocks()->getColumn(mOrigin.x, mOrigin.z).GetHeight() + 1);
      float height_next = static_cast<float>(mWorldHdl->GetWorldBlocks()->getColumn(mOrigin.x + mDelta.x, mOrigin.z + mDelta.z).GetHeight() + 1);
      float max_height = height_now > height_next ? height_now : height_next;

      mOrigin.y = static_cast<int>(max_height);

      mTimer = 0;
   }

   float interp = mTimer / mTimestep;
   Vector3f position(static_cast<float>(mOrigin.x), static_cast<float>(mOrigin.y), static_cast<float>(mOrigin.z));
   Vector3f delta(static_cast<float>(mDelta.x), static_cast<float>(mDelta.y), static_cast<float>(mDelta.z));
   position += delta * interp;
   mPosition->SetPosition(position);

}


void Navigator::Teardown(TickParameters& tp)
{
   delete mPMap;
}

