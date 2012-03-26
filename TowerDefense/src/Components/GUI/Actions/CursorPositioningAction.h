#pragma once
#include "../../Component.h"

class ControlEventReceiver;

class CursorPositioningAction : public Component
{
private:
   ControlEventReceiver* mEventRcvr;

public:
   CursorPositioningAction();

   virtual void Initialise(TickParameters& tp, GameObject* owner);
   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp) {}
};