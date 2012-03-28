#include "CursorPositioningAction.h"
#include "../ControlEventReceiver.h"
#include "../../../GameObject.h"
#include "../../../Messaging/CursorMoveMessage.h"
#include "../../../Log.h"
#include "../../../vmath-collisions.h"
#include "../../../Camera.h"
#include "../../../CommonDefs.h"
#include "../../../WorldBlocks.h"

CursorPositioningAction::CursorPositioningAction(WorldBlocks* world) :
   mWorld(world)
{
}

void CursorPositioningAction::Initialise(TickParameters& tp, GameObject* owner)
{
   Component::Initialise(tp, owner);
   mEventRcvr = owner->GetComponent<ControlEventReceiver>();
}

void CursorPositioningAction::Tick(TickParameters& tp)
{
   if(mEventRcvr && mEventRcvr->GetClicked())
   {
      Vector3f ray_origin;
      Vector3f ray_unit;
      tp.camera->GetRay(Vector2i(mEventRcvr->GetClickX(), mEventRcvr->GetClickY()), ray_origin, ray_unit);

      //Now check if collision occurs on each block
      Vector3f intersect_point;
      Vector3f closest_intersect_point;
      float intersect_distance;
      float min_intersect_distance = FLT_MAX;
      for (int x = 0; x < WORLD_WIDTH; x++)
      {
         for (int z = 0; z < WORLD_BREADTH; z++)
         {
            Column col = mWorld->getColumn(x, z);
            
            for(int y = 0; y < WORLD_HEIGHT; y++)
            {
               Block block = col.Get(y);
               if(block.blockType.Value() == BlockType::Empty)
                  continue;
               if(Collisions3f::RayIntersectsAABB(ray_origin, ray_unit, Vector3f(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)), Vector3f(1.0f, 1.0f, 1.0f), intersect_point, intersect_distance))
               {
                  if(min_intersect_distance > intersect_distance)
                  {
                     closest_intersect_point = intersect_point;
                     min_intersect_distance = intersect_distance;
                  }
               }
            }
         }
      }

      if(min_intersect_distance < FLT_MAX)
      {
         Vector3f floor_intersection(std::floor(closest_intersect_point.x), 
                                     std::floor(closest_intersect_point.y),
                                     std::floor(closest_intersect_point.z));
         tp.msg.GetHub<CursorMoveMessage>().Broadcast(CursorMoveMessage(floor_intersection));
         Log::Debug(__FILE__, "Intersection at (%f,%f,%f)", floor_intersection.x, floor_intersection.y, floor_intersection.z);
      }
   }
}
