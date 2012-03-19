#pragma once

#include "../Component.h"

class GameObject;

class MenuGS : public Component
{
private:
   bool mFirst;
   bool mTransitioningToBuild;
   float mTransitionTimer;
   void SpawnMenuObjects(TickParameters& tp);

   void TransitionToBuild(TickParameters& tp);

public:
   MenuGS();

   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp);

   void StartClick(int x, int y, TickParameters& tp);
   void ExitClick(int x, int y, TickParameters& tp);
};
