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
      TouchData td = mEventRcvr->GetClick();
      tp.camera->GetRay(td.GetTouchCentre(), ray_origin, ray_unit);

      //Now check if collision occurs on each block
      Vector3f intersect_point;
      Vector3f intersection_normal;
      Vector3f closest_intersect_point;
      Vector3f closest_intersect_normal;
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
               
               if(Collisions3f::RayIntersectsAABB(ray_origin, ray_unit, Vector3f(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)), Vector3f(1.0f, 1.0f, 1.0f), intersect_point, intersect_distance, intersection_normal))
               {
                  if(min_intersect_distance > intersect_distance)
                  {
                     closest_intersect_point = Vector3f(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
                     closest_intersect_normal = intersection_normal;
                     min_intersect_distance = intersect_distance;
                  }
               }
            }
         }
      }

      if (min_intersect_distance < FLT_MAX)
      {
         // Cursor to top of stack
         Column col = mWorld->getColumn(static_cast<int>(closest_intersect_point.x), static_cast<int>(closest_intersect_point.z));
         closest_intersect_point.y = static_cast<float>(col.GetHeight()) + 1.0f;
         tp.msg.GetHub<CursorMoveMessage>().Broadcast(CursorMoveMessage(closest_intersect_point));
         Log::Debug(__FILE__, "Intersection at (%f,%f,%f)", closest_intersect_point.x, closest_intersect_point.y, closest_intersect_point.z);
      }
   }
}
