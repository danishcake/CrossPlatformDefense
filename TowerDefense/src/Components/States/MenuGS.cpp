#pragma once

#include "MenuGS.h"
#include "BuildGS.h"
#include "../StateListener.h"
#include "../GUI/Actions/SignalAction.h"
#include "../../Messaging/StateChangeMessage.h"
#include "../../GameObject.h"

#include "../Graphics/Effect.h"
#include "../GUI/ControlArea.h"
#include "../GUI/ControlOutline.h"
#include "../GUI/ControlEventReceiver.h"
#include "../GUI/ControlText.h"
#include "../GUI/ControlTransition.h"
#include "../StateListener.h"
#include "../Graphics/FadeOverlay.h"

static const float TransitionTime = 0.5f;

MenuGS::MenuGS()
   : mFirst(true),
     mTransitionTimer(0),
     mTransitioningToBuild(false)
{
   
}

void MenuGS::Tick(TickParameters& tp)
{
   if(mFirst)
   {
      SpawnMenuObjects(tp);
      mFirst = false;
   }
   if(mTransitioningToBuild)
   {
      mTransitionTimer += tp.timespan;
      if(mTransitionTimer > TransitionTime)
      {
         TransitionToBuild(tp);
      }
   }
}

void MenuGS::Teardown(TickParameters& tp)
{
   
}

void MenuGS::SpawnMenuObjects(TickParameters& tp)
{
   GameObject* button_start = new GameObject();
   button_start->AddComponent(new ControlArea(UDim(Vector2f(0.1f, 0.1f), Vector2f(0.0f, 0.0f), Edge::TopLeft),
                                              UDim(Vector2f(0.8f, 0.2f), Vector2f(0.0f, 0.0f))), tp);
   button_start->AddComponent(new ControlEventReceiver(), tp);
   button_start->AddComponent(new ControlOutline(), tp);
   button_start->AddComponent(new ControlText("Start", "fonts/OrbitronLight.ttf", Vector4f(1, 1, 1, 1)), tp);
   button_start->AddComponent(new SignalAction(boost::bind(&MenuGS::StartClick, this, _1, _2, _3)), tp);
   button_start->AddComponent(new ControlTransition(ControlTransitionState::TransIn, 0.25f, 1), tp);
   button_start->AddComponent(new StateListener(GameStates::Menu, true), tp);
   tp.Spawn(button_start);

   GameObject* button_exit = new GameObject();
   button_exit->AddComponent(new ControlArea(UDim(Vector2f(0.1f, 0.4f), Vector2f(0.0f, 0.0f), Edge::TopLeft),
                                             UDim(Vector2f(0.8f, 0.2f), Vector2f(0.0f, 0.0f))), tp);
   button_exit->AddComponent(new ControlEventReceiver(), tp);
   button_exit->AddComponent(new ControlOutline(), tp);
   button_exit->AddComponent(new ControlText("Exit", "fonts/OrbitronLight.ttf", Vector4f(1, 1, 1, 1)), tp);
   button_exit->AddComponent(new SignalAction(boost::bind(&MenuGS::ExitClick, this, _1, _2, _3)), tp);
   button_exit->AddComponent(new ControlTransition(ControlTransitionState::TransIn, 0.25f, 1), tp);
   button_exit->AddComponent(new StateListener(GameStates::Menu, true), tp);
   tp.Spawn(button_exit);
}

void MenuGS::TransitionToBuild(TickParameters& tp)
{
   GameObject* build_gs = new GameObject();
   build_gs->AddComponent(new BuildGS(0), tp);
   tp.Spawn(build_gs);

   mOwner->Kill();

   tp.msg.GetHub<StateChangeMessage>().Broadcast(StateChangeMessage(GameStates::Build));
}

void MenuGS::StartClick(int x, int y, TickParameters& tp)
{
   if(!mTransitioningToBuild)
   {
      //Spawn a fade object to fade out
      GameObject* fadeGO = new GameObject();
      fadeGO->AddComponent(new FadeOverlay(FadeType::FadeOutThenIn, Vector3f(1, 1, 1), TransitionTime), tp);
      tp.Spawn(fadeGO);

      mTransitioningToBuild = true;

      tp.msg.GetHub<GuiTransitionControlMessage>().Broadcast(GuiTransitionControlMessage(1, GuiTransition::TransitionOut));
   }
}

void MenuGS::ExitClick(int x, int y, TickParameters& tp)
{
   if(!mTransitioningToBuild)
   {
   }
}