#include "BuildGS.h"
#include "../../GameObject.h"
#include "../GUI/Actions/SignalAction.h"
#include "../Graphics/WorldDrawer.h"
#include "../Graphics/PathVisualiser.h"
#include "../Graphics/CursorDrawer.h"
#include "../Position.h"
#include "../CursorEventReceiver.h"
#include "../GUI/ControlArea.h"
#include "../GUI/ControlOutline.h"
#include "../GUI/ControlEventReceiver.h"
#include "../GUI/ControlEventDistributor.h"
#include "../GUI/ControlText.h"
#include "../GUI/ControlTransition.h"
#include "../GUI/ControlProgress.h"
#include "../GUI/Actions/CameraRotateAction.h"
#include "../GUI/Actions/CursorPositioningAction.h"
#include "../StateListener.h"
#include "../../WorldBlocks.h"
#include "DefendGS.h"

static const float TransitionTime = 0.25f;


BuildGS::BuildGS()
   : mFirst(true), mBlocks(NULL), 
     mTransitionTimer(0), mTransitioningToDefend(false),
     mAddProgress(NULL), mRemoveProgress(NULL), mCursorEventRcvr(NULL)
{
   mSharedState.WaveID = 0;
   mSharedState.BlockStock = 5;
   mSharedState.DeleteMoves = 5;
}

BuildGS::BuildGS(WorldBlocks* world, SharedState shared_state)
   : mFirst(true), mBlocks(world), mSharedState(shared_state),
     mTransitionTimer(0), mTransitioningToDefend(false),
     mAddProgress(NULL), mRemoveProgress(NULL), mCursorEventRcvr(NULL)
{
   mSharedState.DeleteMoves = 5;
   mSharedState.BlockStock = 5;
}

void BuildGS::Tick(TickParameters& tp)
{
   if(mFirst)
   {
      SpawnMenuObjects(tp);
      mFirst = false;
   }

   mAddProgress->SetProgress(mSharedState.BlockStock / 5.0f);
   mRemoveProgress->SetProgress(mSharedState.DeleteMoves / 5.0f);
   mCursorEventRcvr->SetDeleteAllowed(mSharedState.DeleteMoves > 0);
   mCursorEventRcvr->SetAddAllowed(mSharedState.BlockStock > 0);



   if (mTransitioningToDefend)
   {
      mTransitionTimer += tp.timespan;
      if (mTransitionTimer > TransitionTime)
      {
         tp.msg.GetHub<StateChangeMessage>().Broadcast(StateChangeMessage(GameStates::Defend));
         GameObject* defend = new GameObject();
         defend->AddComponent(new DefendGS(mBlocks, mSharedState), tp);
         tp.Spawn(defend);
         mOwner->Kill();
      }
   }
}

void BuildGS::Teardown(TickParameters& tp)
{
   
}

void BuildGS::SpawnMenuObjects(TickParameters& tp)
{
   bool from_menu = mBlocks == 0;
   //Load the world
   if(from_menu)
   {
      mBlocks = WorldBlocks::LoadFromPNG(tp, "heightmaps/map3.png");

      //Spawn the world
      GameObject* world = new GameObject();
      world->AddComponent(new WorldDrawer(mBlocks), tp);
      world->AddComponent(new StateListener(GameStates::Build, GameStates::Defend, true), tp);
      tp.Spawn(world);
   }

   //Spawn GUI controls
   //Spawn cursor
   GameObject* cursor = new GameObject();
   cursor->AddComponent(new Position(), tp);
   cursor->AddComponent(new CursorDrawer(), tp);
   mCursorEventRcvr = new CursorEventReceiver(CursorAction::DeleteTop, mBlocks);
   mCursorEventRcvr->SubscribeForAction(boost::bind(&BuildGS::CursorActionPerformed, this, _1, _2));
   cursor->AddComponent(mCursorEventRcvr, tp);
   cursor->AddComponent(new StateListener(GameStates::Build, true), tp);
   tp.Spawn(cursor);

   //Spawn drag area for Camera manipulation, tap detection (with callback)
   GameObject* drag_area = new GameObject();
   drag_area->AddComponent(new ControlArea(UDim(Vector2f(0.0f, 0.0f), Vector2f( 0.0f,  0.0f), Edge::TopLeft),
                                             UDim(Vector2f(1.0f, 0.9f), Vector2f( 0.0f, -20.0f))), tp);
   drag_area->AddComponent(new ControlEventReceiver(), tp);
   drag_area->AddComponent(new CameraRotateAction(CameraAction::TurnAxis), tp);
   drag_area->AddComponent(new CursorPositioningAction(mBlocks), tp);
   drag_area->AddComponent(new StateListener(GameStates::Build, true), tp);
   tp.Spawn(drag_area);

   GameObject* btn_delete = new GameObject();
   btn_delete->AddComponent(new ControlArea(UDim(Vector2f(1.0f, 1.0f), Vector2f(-10.0f, -10.0f), Edge::BottomRight),
                                                UDim(Vector2f(0.15f, 0.1f), Vector2f( 0.0f, 0.0f))), tp);
   btn_delete->AddComponent(new ControlEventReceiver(), tp);
   btn_delete->AddComponent(new ControlOutline(), tp);
   mRemoveProgress = new ControlProgress();
   btn_delete->AddComponent(mRemoveProgress, tp);
   btn_delete->AddComponent(new ControlText("Clr", "fonts/OrbitronLight.ttf", Vector4f(0.707f, 0.137f, 0.137f, 1)), tp);
   btn_delete->AddComponent(new SignalAction(boost::bind(&CursorEventReceiver::SetDeleteMode, mCursorEventRcvr, _1, _2, _3)), tp);
   btn_delete->AddComponent(new ControlTransition(ControlTransitionState::TransIn, TransitionTime, 1), tp);
   btn_delete->AddComponent(new StateListener(GameStates::Build, true), tp);
   tp.Spawn(btn_delete);

   GameObject* btn_add = new GameObject();
   btn_add->AddComponent(new ControlArea(UDim(Vector2f(1.0f, 0.85f), Vector2f(-10.0f, -20.0f), Edge::BottomRight),
                                             UDim(Vector2f(0.15f, 0.1f), Vector2f( 0.0f, 0.0f))), tp);
   btn_add->AddComponent(new ControlEventReceiver(), tp);
   btn_add->AddComponent(new ControlOutline(), tp);
   mAddProgress = new ControlProgress();
   btn_add->AddComponent(mAddProgress, tp);
   btn_add->AddComponent(new ControlText("Add", "fonts/OrbitronLight.ttf", Vector4f(0.137f, 0.707f, 0.137f, 1)), tp);
   btn_add->AddComponent(new SignalAction(boost::bind(&CursorEventReceiver::SetAddMode, mCursorEventRcvr, _1, _2, _3)), tp);
   btn_add->AddComponent(new ControlTransition(ControlTransitionState::TransIn, TransitionTime, 1), tp);
   btn_add->AddComponent(new StateListener(GameStates::Build, true), tp);
   tp.Spawn(btn_add);

   GameObject* btn_go = new GameObject();
   btn_go->AddComponent(new ControlArea(UDim(Vector2f(1.0f, 0.0f), Vector2f(-10.0f, 10.0f), Edge::TopRight),
                                          UDim(Vector2f(0.15f, 0.1f), Vector2f( 0.0f, 0.0f))), tp);
   btn_go->AddComponent(new ControlEventReceiver(), tp);
   btn_go->AddComponent(new ControlOutline(), tp);
   btn_go->AddComponent(new ControlText("GO", "fonts/OrbitronLight.ttf", Vector4f(1.0f, 1.0f, 1.0f, 1)), tp);
   btn_go->AddComponent(new StateListener(GameStates::Build, true), tp);
   btn_go->AddComponent(new ControlTransition(ControlTransitionState::TransIn, TransitionTime, 1), tp);
   btn_go->AddComponent(new SignalAction(boost::bind(&BuildGS::TransitionToDefend, this, _1, _2, _3)), tp);
   tp.Spawn(btn_go);

   GameObject* path_vis = new GameObject();
   path_vis->AddComponent(new PathVisualiser(mBlocks), tp);
   path_vis->AddComponent(new StateListener(GameStates::Build, true), tp);
   tp.Spawn(path_vis);
}


void BuildGS::TransitionToDefend(int x, int y, TickParameters& tp)
{
   tp.msg.GetHub<GuiTransitionControlMessage>().Broadcast(GuiTransitionControlMessage(1, GuiTransition::TransitionOut));
   mTransitioningToDefend = true;
}

void BuildGS::CursorActionPerformed(CursorAction::Enum action, TickParameters& tp)
{
   switch (action)
   {
   case CursorAction::AddTop:
      mSharedState.BlockStock--;
      break;
   case CursorAction::DeleteTop:
      mSharedState.DeleteMoves--;
      break;
   }
}