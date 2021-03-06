#include "DefendGS.h"
#include "../../GameObject.h"
#include "../Graphics/CursorDrawer.h"
#include "../GUI/Actions/CameraRotateAction.h"
#include "../GUI/Actions/SignalAction.h"
#include "../GUI/ControlArea.h"
#include "../GUI/ControlText.h"
#include "../GUI/ControlTransition.h"
#include "../GUI/ControlEventReceiver.h"
#include "../GUI/ControlEventDistributor.h"
#include "../Graphics/CubeDrawer.h"
#include "../Navigator.h"
#include "../WorldHandle.h"
#include "../CursorEventReceiver.h"
#include "../StateListener.h"
#include "../Membership.h"
#include "../BallisticTrajectory.h"
#include "../../WorldBlocks.h"
#include "../Position.h"
#include "../../Camera.h"
#include "../../vmath-collisions.h"
#include "../SendOnDeath.h"
#include "BuildGS.h"
#include <sstream>
#include <iomanip>


static const float TransitionTime = 0.25f;

DefendGS::DefendGS(WorldBlocks* world, SharedState state)
   : mFirst(true), mBlocks(world), mSharedState(state),
     mSpawnCount(0), mDeathCount(0),
     mTransitioningToBuild(false), mTransitionTimer(0.0f)
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
      UpdateWalkerText(tp);
   }


   mMaxSpawn = (mSharedState.WaveID + 2) * 3;
   mMaxSpawn = std::min(mMaxSpawn, 100);
   float spawn_interval = 1.5f / (mSharedState.WaveID + 4);
   spawn_interval = std::max(spawn_interval, 0.2f);

   if (mSpawnCount < mMaxSpawn)
   {
      if (mSpawnTime <= 0)
      {
         mSpawnTime = spawn_interval;
         SpawnWalker(tp);
         UpdateWalkerText(tp);
      }
   }

   if (mDeathCount >= mMaxSpawn && !mTransitioningToBuild)
   {
      mTransitioningToBuild = true;
      tp.msg.GetHub<GuiTransitionControlMessage>().Broadcast(GuiTransitionControlMessage(1, GuiTransition::TransitionOut));
   }

   if (mTransitioningToBuild)
   {
      mTransitionTimer += tp.timespan;
      if (mTransitionTimer >= TransitionTime)
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


   //Generate position around the edge - index 0,0 as 0, the progress down x 
   int pos_index = rand() % (WORLD_WIDTH * 2 + WORLD_BREADTH * 2);
   Vector3f spawn_position;
   if (pos_index < WORLD_WIDTH)
   {
      spawn_position.x = static_cast<float>(pos_index);
      spawn_position.z = 0.0f;
   } else if (pos_index < WORLD_WIDTH + WORLD_BREADTH)
   {
      spawn_position.x = static_cast<float>(WORLD_WIDTH - 1);
      spawn_position.z = static_cast<float>(pos_index - WORLD_WIDTH);
   } else if (pos_index < 2 * WORLD_WIDTH + WORLD_BREADTH)
   {
      spawn_position.x = static_cast<float>(pos_index - (WORLD_WIDTH + WORLD_BREADTH));
      spawn_position.z = static_cast<float>(WORLD_BREADTH - 1);
   } else
   {
      spawn_position.x = 0.0f;
      spawn_position.z = static_cast<float>(pos_index - (2 * WORLD_WIDTH + WORLD_BREADTH));
   }
    
   Position* pos = new Position();
   pos->SetPosition(spawn_position);
   walker->AddComponent(pos, tp);
   walker->AddComponent(new Navigator(), tp);
   walker->AddComponent(new CubeDrawer(), tp);
   walker->AddComponent(new WorldHandle(mBlocks), tp);
   walker->AddComponent(new StateListener(GameStates::Build, GameStates::Defend, true), tp); //Ditto make defend only
   walker->AddComponent(new Membership<MembershipType::BadGuy>(), tp);
   walker->AddComponent(new SendOnDeath<WalkerDeathMessage>(), tp);
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

   GameObject* walker_counter = new GameObject();
   walker_counter->AddComponent(new ControlArea(UDim(Vector2f(1.0f, 0.0f), Vector2f( -10.0f, 10.0f), Edge::TopRight),
                                                UDim(Vector2f(0.3f, 0.05f), Vector2f( 0.0f, 0.0f))), tp);
   mWalkerCountText = new ControlText("000/000", "fonts/OrbitronLight.ttf", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
   walker_counter->AddComponent(mWalkerCountText, tp);
   walker_counter->AddComponent(new StateListener(GameStates::Defend, true), tp);
   walker_counter->AddComponent(new ControlTransition(ControlTransitionState::TransIn, TransitionTime, 1), tp);
   tp.Spawn(walker_counter);
}

void DefendGS::UpdateWalkerText(TickParameters& tp)
{
   std::ostringstream stm;
   stm << std::setfill('0') << std::setw(3) << mSpawnCount << "/" <<
          std::setfill('0') << std::setw(3) << mMaxSpawn;

   mWalkerCountText->SetText(tp, stm.str());
}

void DefendGS::TransitionToBuild(TickParameters& tp)
{
   tp.msg.GetHub<StateChangeMessage>().Broadcast(StateChangeMessage(GameStates::Build));

   GameObject* build = new GameObject();
   build->AddComponent(new BuildGS(mBlocks, mSharedState), tp);
   tp.Spawn(build);
   mOwner->Kill();
}

void DefendGS::TapToKill(int x, int y, TickParameters& tp)
{
   // Spawn a bomb object
   GameObject* bomb = new GameObject();
   bomb->AddComponent(new Position(), tp);
   bomb->AddComponent(new CubeDrawer(), tp);
   bomb->AddComponent(new BallisticTrajectory(Vector2i(x, y), 0.25f), tp);
   bomb->AddComponent(new WorldHandle(mBlocks), tp);

   tp.Spawn(bomb);
}
