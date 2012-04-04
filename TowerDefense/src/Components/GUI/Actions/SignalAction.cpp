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
      TouchData td = mEventRcvr->GetClick();
      mOnClick(td.GetTouchCentre().x, td.GetTouchCentre().y, tp);
   }
}
