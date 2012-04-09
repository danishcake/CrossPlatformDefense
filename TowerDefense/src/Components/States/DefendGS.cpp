#include "DefendGS.h"
#include "../../GameObject.h"
#include "../Graphics/CursorDrawer.h"
#include "../GUI/Actions/CameraRotateAction.h"
#include "../GUI/Actions/SignalAction.h"
#include "../GUI/ControlArea.h"
#include "../GUI/ControlEventReceiver.h"
#include "../GUI/ControlEventDistributor.h"
#include "../Graphics/CubeDrawer.h"
#include "../Navigator.h"
#include "../WorldHandle.h"
#include "../CursorEventReceiver.h"
#include "../StateListener.h"
#include "../Membership.h"
#include "../../WorldBlocks.h"
#include "../Position.h"
#include "../../Camera.h"
#include "../../vmath-collisions.h"
#include "BuildGS.h"


DefendGS::DefendGS(WorldBlocks* world, SharedState state)
   : mFirst(true), mBlocks(world), mSharedState(state),
     mSpawnCount(0), mDeathCount(0), mBackToBuildTimer(0.0f)
{
   mSpawnTime = 3.0f; // First spawn in 3s
}

void DefendGS::Initialise(TickParameters& tp, GameObject* owner)
{
   mDeathSubscriber.Initialise(&tp.msg);
   Component::Initialise(tp, owner);
}

void DefendGS::Tick(TickParameters& tp)
{
   if(mFirst)
   {
      mSharedState.WaveID++;
      SpawnObjects(tp);
      mFirst = false;
   }

   mSpawnTime -= tp.timespan;

   // Count all the deaths
   std::vector<WalkerDeathMessage> deaths = mDeathSubscriber.GetMessages();
   for(std::vector<WalkerDeathMessage>::iterator it = deaths.begin(); it != deaths.end(); ++it)
   {
      mDeathCount++;
   }


   int max_spawn = (mSharedState.WaveID + 4) * 3;
   max_spawn = std::min(max_spawn, 100);
   float spawn_interval = 10.0f / (mSharedState.WaveID + 4);
   spawn_interval = std::max(spawn_interval, 0.2f);

   if (mSpawnCount < max_spawn)
   {
      if (mSpawnTime <= 0)
      {
         mSpawnTime = spawn_interval;
         SpawnWalker(tp);
      }
   }

   if(mDeathCount >= max_spawn)
   {
      float ltv_back_timer = mBackToBuildTimer;
      mBackToBuildTimer += tp.timespan;
      if (mBackToBuildTimer >= 2.0f && ltv_back_timer < 2.0f)
      {
         TransitionToBuild(tp);
      }
   }
}

void DefendGS::Teardown(TickParameters& tp)
{
   
}

void DefendGS::SpawnWalker(TickParameters& tp)
{
   GameObject* walker = new GameObject();
   Position* pos = new Position();
   pos->SetPosition(Vector3f(0, 0, 0));
   walker->AddComponent(pos, tp);
   walker->AddComponent(new Navigator(), tp);
   walker->AddComponent(new CubeDrawer(), tp);
   walker->AddComponent(new WorldHandle(mBlocks), tp);
   walker->AddComponent(new StateListener(GameStates::Build, GameStates::Defend, true), tp); //Ditto make defend only
   walker->AddComponent(new Membership<MembershipType::BadGuy>(), tp);
   tp.Spawn(walker);

   mSpawnCount++;
}

void DefendGS::SpawnObjects(TickParameters& tp)
{
   //Spawn drag area for Camera manipulation, tap detection (with callback)
   GameObject* drag_area = new GameObject();
   drag_area->AddComponent(new ControlArea(UDim(Vector2f(0.0f, 0.0f), Vector2f( 0.0f,  0.0f), Edge::TopLeft),
                                             UDim(Vector2f(1.0f, 0.9f), Vector2f( 0.0f, -20.0f))), tp);
   drag_area->AddComponent(new ControlEventReceiver(), tp);
   drag_area->AddComponent(new CameraRotateAction(CameraAction::TurnAxis), tp);
   drag_area->AddComponent(new SignalAction(boost::bind(&DefendGS::TapToKill, this, _1, _2, _3)), tp);
   drag_area->AddComponent(new StateListener(GameStates::Defend, true), tp);
   tp.Spawn(drag_area);
}

void DefendGS::TransitionToBuild(TickParameters& tp)
{
   tp.msg.GetHub<StateChangeMessage>().Broadcast(StateChangeMessage(GameStates::Build));
   
   GameObject* build = new GameObject();
   build->AddComponent(new BuildGS(mBlocks, mSharedState), tp);
   tp.Spawn(build);
}

void DefendGS::TapToKill(int x, int y, TickParameters& tp)
{
   // Obtain a ray cast from the camera
   Vector3f ray_origin, ray_unit;
   tp.camera->GetRay(Vector2i(x, y), ray_origin, ray_unit);

   // Check for intersection with any walkers
   // TODO: Perhaps chuck a projectile instead?
   std::vector<GameObject*> walkers = tp.memberships.GetMembers(MembershipType::BadGuy);
   for (std::vector<GameObject*>::iterator it = walkers.begin(); it != walkers.end(); ++it)
   {
      Position* pos = (*it)->GetComponent<Position>();
      if (pos)
      {
         if (Collisions3f::RayIntersectsSphere(ray_origin, ray_unit, pos->GetPosition() + Vector3f(0.5f, 0.5f, 0.5f), 1.0f))
         {
            (*it)->Kill();
         }
      }
   }
}
