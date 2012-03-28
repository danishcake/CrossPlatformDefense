#pragma once
#include "../../Component.h"

class ControlEventReceiver;
class WorldBlocks;

class CursorPositioningAction : public Component
{
private:
   ControlEventReceiver* mEventRcvr;
   WorldBlocks* mWorld;

public:
   CursorPositioningAction(WorldBlocks* world);

   virtual void Initialise(TickParameters& tp, GameObject* owner);
   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp) {}
};