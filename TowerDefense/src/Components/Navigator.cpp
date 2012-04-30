#include "Navigator.h"
#include "../PotentialMap.h"
#include "../GameObject.h"
#include "Position.h"
#include "WorldHandle.h"
#include "../WorldBlocks.h"
#include "../Log.h"
#include <cmath>


Navigator::Navigator()
 : mTimer(0), mTimestep(0.1f), mAction(NavMode::Walking)
{
}

void Navigator::Initialise(TickParameters& tp, GameObject* owner)
{
   Component::Initialise(tp, owner);
   mPosition =  owner->GetComponent<Position>();
   mWorldHdl = owner->GetComponent<WorldHandle>();


   mPMap = new PotentialMap(mWorldHdl->GetWorldBlocks(), 1, 1);
   Vector3f position = mPosition->GetPosition();
   position.y = static_cast<float>(mWorldHdl->GetWorldBlocks()->getColumn(0, 0).GetHeight() + 1);
   mPosition->SetPosition(position);
   mOrigin = mPosition->GetPosition();
   mInvalidationWatcher.CheckInvalidation(*mWorldHdl->GetWorldBlocks());
}


bool Navigator::FollowPotential()
{
   // I'm paranoid about height change, ensure zero
   mDelta.y = 0; 
   mOrigin += mDelta;

   mDelta = Vector3f();
   //Fall down, step up
   int height_now  = mWorldHdl->GetWorldBlocks()->getColumn(mOrigin.x, mOrigin.z).GetHeight() + 1;
   int height_next = mWorldHdl->GetWorldBlocks()->getColumn(mOrigin.x + mDelta.x, mOrigin.z + mDelta.z).GetHeight() + 1;
   mOrigin.y = static_cast<int>(std::max(height_now, height_next));

   if (mPMap->Guide(mOrigin.x, mOrigin.z, mDelta.x, mDelta.z))
   {
      return true;
   } else
   {
      return false;
   }
}

void Navigator::HandleStuck()
{
   // Navigation stuck, in an isolated part of the map
   // Dig/Build towards the nearest navigable bit
   Vector2i nearest_navigable;
   mPMap->NearestNavigable(mOrigin.x, mOrigin.z, nearest_navigable.x, nearest_navigable.y);
   Vector2i direction_to_move(nearest_navigable.x - mOrigin.x, nearest_navigable.y - mOrigin.z);

   // Now if I can move in this direction in either axis then move that way
   // Otherwise dig/build along longest axis


   Vector2i options[2];
   int option_count = 0;
   if(abs(direction_to_move.x) == abs(direction_to_move.y) && direction_to_move.x != 0)
   {
      options[0] = Vector2i(direction_to_move.x > 0 ? 1 : -1, 0);
      options[1] = Vector2i(0, direction_to_move.y > 0 ? 1 : -1);
      // No reason to pick on or the other, so swap 50% of the time
      if(rand() % 2) std::swap(options[0], options[1]);
      option_count = 2;
   } else if(abs(direction_to_move.x) > abs(direction_to_move.y))
   {
      options[option_count++] = Vector2i(direction_to_move.x > 0 ? 1 : -1, 0);
      if (direction_to_move.y != 0)
         options[option_count++] = Vector2i(0, direction_to_move.y > 0 ? 1 : -1);
   } else if(abs(direction_to_move.y) > abs(direction_to_move.x))
   {
      options[option_count++] = Vector2i(0, direction_to_move.y > 0 ? 1 : -1);
      if (direction_to_move.x != 0)
         options[option_count++] = Vector2i(direction_to_move.x > 0 ? 1 : -1, 0);
   } else
   {
      // Both zero, no options :(
      Log::Error(__FILE__, "Unable to find a route, killing walker");
      mOwner->Kill();
   }

   // 20% chance of taking the worse route if available
   int selected_option = 0;
   if (option_count > 0 && rand() % 5 == 0)
   {
      selected_option = 1;
   }

   // If the selected option height is too far then level it towards it
   Vector2i selected_position(mOrigin.x + options[selected_option].x, mOrigin.z + options[selected_option].y);
   Column dest_column = mWorldHdl->GetWorldBlocks()->getColumn(selected_position.x, selected_position.y);
   Column current_column = mWorldHdl->GetWorldBlocks()->getColumn(mOrigin.x, mOrigin.z);
   int dest_column_height = dest_column .GetHeight();
   int src_column_height = current_column.GetHeight();

   if (dest_column_height > src_column_height + 1)
   {
      mAction = NavMode::Building;
      mDelta = Vector3i();
   }
   else if (dest_column_height < src_column_height - 1)
   {
      mAction = NavMode::Digging;
      mDelta = Vector3i();
   } else
   {
      mAction = NavMode::Walking;
      mDelta = Vector3i(options[selected_option].x, 0, options[selected_option].y);
   }
}

void Navigator::DigThrough()
{
   mAction = NavMode::Walking;
   Column current_position = mWorldHdl->GetWorldBlocks()->getColumn(mOrigin.x, mOrigin.z);
   current_position.Set(current_position.GetHeight(), BlockType::Empty);
   if(current_position.GetHeight() > 0)
      current_position.Set(current_position.GetHeight(), BlockType::Grass);

   mOrigin.y = static_cast<int>(current_position.GetHeight() + 1);
   mDelta = Vector3i();
   if (!FollowPotential())
   {
      HandleStuck();
   }
}

void Navigator::BuildOver()
{
   mAction = NavMode::Walking;
   Column current_position = mWorldHdl->GetWorldBlocks()->getColumn(mOrigin.x, mOrigin.z);
   if(current_position.GetHeight() > 0)
      current_position.Set(current_position.GetHeight(), BlockType::Dirt);
   current_position.Set(current_position.GetHeight() + 1, BlockType::Grass);

   mOrigin.y = static_cast<int>(current_position.GetHeight() + 1);
   mDelta = Vector3i();
   if (!FollowPotential())
   {
      HandleStuck();
   }
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
      mTimer = 0;

      if (mAction == NavMode::Walking)
      {
         if (!FollowPotential())
         {
            HandleStuck();
         }
      } else if (mAction == NavMode::Digging)
      {
         DigThrough();
      } else if (mAction == NavMode::Building)
      {
         BuildOver();
      }
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

