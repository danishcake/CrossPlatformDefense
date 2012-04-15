#pragma once
#include "Component.h"
#include "../GameObject.h"


/*
 * SendOnDeath broadcasts specified message when owning 
 * GameObject dies
 */
template <class T> class SendOnDeath : public Component
{
private:
public:
   virtual void Tick(TickParameters& tp) {}
   virtual void Teardown(TickParameters& tp)
   {
      tp.msg.GetHub<T>().Broadcast(T());
   }
};
