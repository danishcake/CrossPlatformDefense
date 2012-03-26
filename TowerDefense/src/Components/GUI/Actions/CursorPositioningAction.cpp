#include "CursorPositioningAction.h"
#include "../ControlEventReceiver.h"
#include "../../../GameObject.h"
#include "../../../Messaging/CursorMoveMessage.h"
#include "../../../Log.h"


CursorPositioningAction::CursorPositioningAction()
{
}

void CursorPositioningAction::Initialise(TickParameters& tp, GameObject* owner)
{
   Component::Initialise(tp, owner);
   mEventRcvr = owner->GetComponent<ControlEventReceiver>();
}

void CursorPositioningAction::Tick(TickParameters& tp)
{
   if(mEventRcvr && mEventRcvr->GetClicked())
   {
      Log::Debug(__FILE__, "Click!");
      //tp.msg.GetHub<CursorMoveMessage>().Broadcast(CursorMoveMessage(Vector3f(
   }
}
