#pragma once

#include "../Component.h"

class GameObject;
class WorldBlocks;

class BuildGS : public Component
{
private:
   bool mFirst;
   bool mFromMenu;
   void SpawnMenuObjects(TickParameters& tp);
   WorldBlocks* mBlocks;

public:
   BuildGS(WorldBlocks* world);
   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp);
};
