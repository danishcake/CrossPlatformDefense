#include "CameraRotateAction.h"
#include "../ControlEventReceiver.h"
#include "../../../GameObject.h"
#include "../../../Camera.h"

CameraRotateAction::CameraRotateAction(CameraAction::Enum action)
   : mAction(action)
{

}

void CameraRotateAction::Initialise(TickParameters& tp, GameObject* owner)
{
   Component::Initialise(tp, owner);
   mEventRcvr = owner->GetComponent<ControlEventReceiver>();
}

void CameraRotateAction::Tick(TickParameters& tp)
{
   if(mEventRcvr)
   {
      if(mEventRcvr->GetDrag() && mAction == CameraAction::TurnAxis)
      {
         tp.camera->RotateAxis(static_cast<float>(mEventRcvr->GetDragDX()), static_cast<float>(mEventRcvr->GetDragDY()));
      }
      if(mEventRcvr->GetClicked())
      {
         if(mAction == CameraAction::TurnLeft)
            tp.camera->RotateLeft();
         else if(mAction == CameraAction::TurnRight)
            tp.camera->RotateRight();
         else if(mAction == CameraAction::ZoomIn)
            tp.camera->ZoomIn();
         else if(mAction == CameraAction::ZoomOut)
            tp.camera->ZoomIn();
         else if(mAction == CameraAction::ZoomToggle)
            tp.camera->ZoomToggle();
      }
   }
}
