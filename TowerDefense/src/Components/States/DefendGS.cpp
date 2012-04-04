#include "DefendGS.h"
#include "../../GameObject.h"
#include "../StateListener.h"
#include "../../WorldBlocks.h"
#include "../Position.h"


DefendGS::DefendGS(WorldBlocks* world)
   : mFirst(true), mBlocks(world)
{
}

void DefendGS::Tick(TickParameters& tp)
{
   if(mFirst)
   {
      SpawnObjects(tp);
      mFirst = false;
   }
}

void DefendGS::Teardown(TickParameters& tp)
{
   
}

void DefendGS::SpawnObjects(TickParameters& tp)
{
   //Spawn GUI controls

   //Spawn a walker (move to defend GS when done)
	/*
   GameObject* walker = new GameObject();
   Position* pos = new Position();
   pos->SetPosition(Vector3f(0, 0, 0));
   walker->AddComponent(pos, tp);
   walker->AddComponent(new Navigator(), tp);
   walker->AddComponent(new CubeDrawer(), tp);
   walker->AddComponent(new WorldHandle(mBlocks), tp);
   walker->AddComponent(new StateListener(GameStates::Defend, true), tp); //Ditto make defend only
   tp.Spawn(walker);*/
}
