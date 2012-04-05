#include "CameraRotateAction.h"
#include "../ControlEventReceiver.h"
#include "../ControlText.h"
#include "../../../GameObject.h"
#include "../../../Camera.h"
#include "../../../Log.h"

CameraRotateAction::CameraRotateAction(CameraAction::Enum action)
   : mAction(action), mLastDoubleTouch(false), mStartDoubleTouchDistance(0)
{

}

void CameraRotateAction::Initialise(TickParameters& tp, GameObject* owner)
{
   Component::Initialise(tp, owner);
   mEventRcvr = owner->GetComponent<ControlEventReceiver>();
   mText = owner->GetComponent<ControlText>();
}

void CameraRotateAction::Tick(TickParameters& tp)
{
   if(mEventRcvr)
   {
      if(mEventRcvr->GetDrag() && mAction == CameraAction::TurnAxis)
      {
         TouchData td = mEventRcvr->GetDragCurrent();
         TouchData td_prev = mEventRcvr->GetDragPrev();
         Vector2i delta = td.GetTouchCentre() - td_prev.GetTouchCentre();

         if(td.GetTouchCount() == 1)
         {
            tp.camera->RotateAxis(static_cast<float>(delta.x), static_cast<float>(delta.y));
            mLastDoubleTouch = false;
         } else if(td.GetTouchCount() == 2)
         {
            Log::Debug(__FILE__, "Pan by (%d, %d)", delta.x, delta.y);


            float touch_distance = td.GetTouchDistance();
            if(!mLastDoubleTouch)
            {
               mStartDoubleTouchDistance = touch_distance;
               mTouchDistanceThresholdReached = false;
            }

            if(std::abs(touch_distance - mStartDoubleTouchDistance) > 30)
            {
               mTouchDistanceThresholdReached = true;
            }

            if(mTouchDistanceThresholdReached)
            {
               tp.camera->ZoomScale(20.0f * (mLastDoubleTouchDistance - touch_distance) / mStartDoubleTouchDistance);
            } else
            {
               tp.camera->Pan(static_cast<float>(td.GetTouchCentre().x), static_cast<float>(td.GetTouchCentre().y), static_cast<float>(delta.x), static_cast<float>(delta.y));
            }

            mLastDoubleTouchDistance = touch_distance;
            mLastDoubleTouch = true;
         }
      } else
      {
         mLastDoubleTouch = false;
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
         {
            tp.camera->ZoomToggle();
            if(mText)
            {
               if(tp.camera->IsZoomed())
               {
                  mText->SetText(tp, "-");
               } else
               {
                  mText->SetText(tp, "+");
               }
            }
         }
         else if(mAction == CameraAction::PanRotateToggle)
         {
            tp.camera->PanRotateToggle();
            if(mText)
            {
               if(tp.camera->IsPanning())
               {
                  mText->SetText(tp, "P");
               } else
               {
                  mText->SetText(tp, "R");
               }
            }
         }

      }
   }
}
