#pragma once

#include "TickParameters.h"
#include <vector>

using std::vector;
class Component;

/*
 * Root game object class
 * Contains a list of Components that define the behaviour of the object
 * Lifecycle of object is: 
 *
 * Initialise called once
 * InitialiseGraphics called once
 * ... loop
 * Tick called
 * Draw called
 * ... loop
 * TeardownGraphics called once
 * Teardown called once
 *
 * GameObject will be removed once marked dead, and Draw will not be called 
 * on that pass.
 */
class GameObject
{
public:
   GameObject();

   void Initialise(TickParameters& tp);
   void InitialiseGraphics(TickParameters& tp);
   void Tick(TickParameters& tp);
   void Draw(TickParameters& tp);
   void Teardown(TickParameters& tp);
   void TeardownGraphics(TickParameters& tp);

   void Kill() {mIsDead = true;}
   static bool IsDead(GameObject* go) {return go->mIsDead;}

   /*
    * Templated method to find other components. Allows a component to call
    * mOwner.GetComponent<Position> to return a pointer to any position
    * component
    */
   template <class T> T* GetComponent()
   {
      for(vector<Component*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
      {
         T* cast_component = dynamic_cast<T*>(*it);
         if(cast_component)
         {
            return cast_component;
         }
      }
      return 0;
   }

   /*
    * Adds a new component. If GameObject has been initialised then it will
    * initialise automatically, otherwise will leave it for GameObject::Initialise
    */
   void AddComponent(Component* component, TickParameters& tp);
   /*
    * Removes a component. If it has been initialised it will be torn down.
    */
   void RemoveComponent(Component* component, TickParameters& tp);
private:
   bool mIsDead;
   bool mInitialised;
   vector<Component*> mComponents;
};
