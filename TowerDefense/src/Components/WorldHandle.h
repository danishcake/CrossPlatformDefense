#pragma once

#include "Component.h"

class WorldBlocks;

/* WorldHandle
 * This component is used to contain a reference to the WorldBlocks. It can then be
 * discovered with GameObject::GetComponent<WorldHandle>() by other components that 
 * want to get a pointer to the world
 */
class WorldHandle : public Component
{
public:
   WorldHandle(WorldBlocks* wb) {mWorldBlocks = wb;}

   virtual void Tick(TickParameters& tp) {}
   virtual void Teardown(TickParameters& tp) {}

   WorldBlocks* GetWorldBlocks() {return mWorldBlocks;}
private:
   WorldBlocks* mWorldBlocks;
};