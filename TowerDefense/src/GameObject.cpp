#include "GameObject.h"
#include "Components/Component.h"
#include <algorithm>

GameObject::GameObject()
: mIsDead(false), mInitialised(false)
{
}

void GameObject::Initialise(TickParameters& tp)
{
   //Calls initialise on all components, setting their owner
   for(vector<Component*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
   {
      (*it)->Initialise(tp, this);
   }
   mInitialised = true;
}

void GameObject::InitialiseGraphics(TickParameters& tp)
{
   for(vector<Component*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
   {
      (*it)->InitialiseGraphics(tp);
   }
}

void GameObject::Teardown(TickParameters& tp)
{
   //Calls teardown on all components
   for(vector<Component*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
   {
      (*it)->Teardown(tp);
      delete *it;
   }
   mComponents.clear();
}

void GameObject::TeardownGraphics(TickParameters& tp)
{
   for(vector<Component*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
   {
      (*it)->TeardownGraphics(tp);
   }
}

void GameObject::Tick(TickParameters& tp)
{
   //Calls tick on all components
   for(vector<Component*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
   {
      (*it)->Tick(tp);
   }
}

void GameObject::Draw(TickParameters& tp)
{
   //Calls draw on all components
   for(vector<Component*>::iterator it = mComponents.begin(); it != mComponents.end(); ++it)
   {
      (*it)->Draw(tp);
   }
}

void GameObject::AddComponent(Component* component, TickParameters& tp)
{
   mComponents.push_back(component);
   if(mInitialised) //Initialise called automatically if GameObject not initialised
      component->Initialise(tp, this);

}

void GameObject::RemoveComponent(Component* component, TickParameters& tp)
{
   //Remove the element from the component and tear it down if found and initialised
   std::size_t size = mComponents.size();

   mComponents.erase(std::remove(mComponents.begin(), mComponents.end(), component), mComponents.end());
   if(mComponents.size() < size && mInitialised)
   {
      component->Teardown(tp);
   }
}
