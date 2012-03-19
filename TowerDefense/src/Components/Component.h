#pragma once

#include "../TickParameters.h"

class GameObject;

class Component
{
public:
   virtual void Initialise(TickParameters& tp, GameObject* owner) { mOwner = owner; } //Will need to be overridden too!
   virtual void InitialiseGraphics(TickParameters& tp) {} //Will need to be overriden in graphical components
   virtual void Tick(TickParameters& tp) = 0;
   virtual void Draw(TickParameters& tp) {}; //Optional method
   virtual void Teardown(TickParameters& tp) = 0;
   virtual void TeardownGraphics(TickParameters& tp) {} //Will need to be overriden in graphical components. Gets called before Teardown

   virtual ~Component(){}
protected:
   GameObject* mOwner;
};
