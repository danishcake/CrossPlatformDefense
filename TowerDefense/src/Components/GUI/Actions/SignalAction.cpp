#pragma once
#include "SignalAction.h"
#include "../../../GameObject.h"
#include "../ControlEventReceiver.h"

SignalAction::SignalAction(const OnClickSlotType& slot)
   : mEventRcvr(0)
{
   mOnClick.connect(slot);
}


void SignalAction::Initialise(TickParameters& tp, GameObject* owner)
{
   Component::Initialise(tp, owner);
   mEventRcvr = owner->GetComponent<ControlEventReceiver>();
}

void SignalAction::Tick(TickParameters& tp)
{
   if(mEventRcvr && mEventRcvr->GetClicked())
   {
      mOnClick(mEventRcvr->GetClickX(), mEventRcvr->GetClickY(), tp);
   }
}
