#include "BallisticTrajectory.h"
#include "../GameObject.h"
#include "Position.h"
#include "WorldHandle.h"
#include "../Camera.h"
#include "../Log.h"
#include "../WorldBlocks.h"



//   Position* mPosition;
//   Vector3f mVelocity;

void BallisticTrajectory::Initialise(TickParameters& tp, GameObject* owner)
{
   Component::Initialise(tp, owner);
   mPosition = mOwner->GetComponent<Position>();
   mWorldHandle = mOwner->GetComponent<WorldHandle>();

   if (!mPosition)
   {
      Log::Error(__FILE__, "BallisticTrajectory used in GameObject without a Position component, killing owner");
      mOwner->Kill();
   }


   // Populate spline to follow
   Vector3f ray_origin, ray_unit, intersection_point;
   tp.camera->GetRay(mClickPosition, ray_origin, ray_unit);
   mSpline.AddPoint(ray_origin, 0.0f);

   if (!mWorldHandle->GetWorldBlocks()->CheckRayIntersection(ray_origin, ray_unit, &intersection_point))
   {
      // Intersection point defaulted to zero altitude
      intersection_point = ray_origin + ray_unit * (ray_origin.y / ray_unit.y);
      // TODO handle misclicks that are miles away
      // TODO handle ray_unit.y close to zero
   }

   Vector3f midpoint = (ray_origin + intersection_point) * 0.5f;
   midpoint.y = ray_origin.y * 0.75f + intersection_point.y * 0.25f;
   mSpline.AddPoint(midpoint, 0.5f * mTimeOfFlight);
   mSpline.AddPoint(intersection_point, mTimeOfFlight);
   mSpline.Interpolator().SetTension(1.0f);

   mTimer = 0;
}

BallisticTrajectory::BallisticTrajectory(Vector2i screen_click, float time_of_flight) :
   mClickPosition(screen_click),
   mTimeOfFlight(time_of_flight)
{
}

void BallisticTrajectory::Tick(TickParameters& tp)
{
   mTimer += tp.timespan;
   if (mTimer > mTimeOfFlight)
   {
      mOwner->Kill();
   }
   mPosition->SetPosition(mSpline.GetValue(mTimer));
}

