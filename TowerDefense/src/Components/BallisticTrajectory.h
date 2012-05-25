#pragma once
#include "Component.h"
#include "../vmath-curves.h"

class Position;
class WorldHandle;

// Bit of a misnomer. This is fired towards a known point, and the initial
// velocity is adjusted so that it hits 1s later
class BallisticTrajectory : public Component
{
private:
   Position* mPosition;
   WorldHandle* mWorldHandle;

   float mTimer;
   float mTimeOfFlight;
   CardinalSpline3f mSpline;

   Vector2i mClickPosition;

public:
   BallisticTrajectory(Vector2i screen_click, float time_of_flight);

   virtual void Initialise(TickParameters& tp, GameObject* owner);
   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp) {}
};
