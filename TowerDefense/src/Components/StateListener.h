#pragma once
#include "Component.h"
#include "../Messaging/StateChangeMessage.h"
#include <set>

class StateListener : public Component
{
private:
   bool mEnabled;
   bool mKillOwner;
   std::set<GameStates::Enum> mActiveStates;
   Subscriber<StateChangeMessage> mStateChangeSubscriber;

public:
   StateListener(GameStates::Enum active_state, bool kill_owner = false);
   StateListener(GameStates::Enum active_state0, GameStates::Enum active_state1, bool kill_owner = false);
   StateListener(GameStates::Enum active_state0, GameStates::Enum active_state1, GameStates::Enum active_state2, bool kill_owner = false);

   virtual void Initialise(TickParameters& tp, GameObject* owner);
   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp) {}
};
