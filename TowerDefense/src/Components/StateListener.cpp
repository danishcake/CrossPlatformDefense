#include "StateListener.h"
#include "../GameObject.h"


StateListener::StateListener(GameStates::Enum active_state, bool kill_owner)
   : mEnabled(false), mKillOwner(kill_owner)
{
   mActiveStates.insert(active_state);
   //mActiveStates.emplace(active_state);
}

StateListener::StateListener(GameStates::Enum active_state0, GameStates::Enum active_state1, bool kill_owner)
   : mEnabled(false), mKillOwner(kill_owner)
{
   mActiveStates.insert(active_state0);
   mActiveStates.insert(active_state1);
}

StateListener::StateListener(GameStates::Enum active_state0, GameStates::Enum active_state1, GameStates::Enum active_state2, bool kill_owner)
   : mEnabled(false), mKillOwner(kill_owner)
{
   mActiveStates.insert(active_state0);
   mActiveStates.insert(active_state1);
   mActiveStates.insert(active_state2);
}

void StateListener::Initialise(TickParameters& tp, GameObject* owner)
{
   mStateChangeSubscriber.Initialise(&tp.msg);
   Component::Initialise(tp, owner);
}

void StateListener::Tick(TickParameters& tp)
{
   std::vector<StateChangeMessage> messages = mStateChangeSubscriber.GetMessages();

   if(messages.size() > 0)
   {
      mEnabled = false;
   
      for(std::vector<StateChangeMessage>::iterator it = messages.begin(); it != messages.end(); ++it)
      {
         if(mActiveStates.count(it->game_state))
            mEnabled = true;
      }
      
      if(mKillOwner && !mEnabled)
      {
         mOwner->Kill();
      }
   }
}
