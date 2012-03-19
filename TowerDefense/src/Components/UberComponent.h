#pragma once

#include "Component.h"
#include "../Messaging/MessageHub.h"


/*
 * Special component for testing lots of features in
 */
class UberComponent : public Component
{
public:
   virtual void Initialise(TickParameters& tp, GameObject* owner)
   {
      mOwner = owner;
      mSubscriber.Initialise(&tp.msg);
   }

   virtual void Tick(TickParameters& tp)
   {
   }

   virtual void Teardown(TickParameters& tp)
   {

   }

private:
   Subscriber<int> mSubscriber;
};