#pragma once
#include "../Component.h"

class ControlEventDistributor : public Component
{
private:

public:
   ControlEventDistributor();

   virtual void Initialise(TickParameters& tp, GameObject* owner);
   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp);
};