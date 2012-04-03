#pragma once
#include "../../Component.h"


namespace CameraAction
{
   enum Enum
   {
      TurnLeft, TurnRight, TurnAxis, ZoomIn, ZoomOut, ZoomToggle, PanRotateToggle
   };
}

class ControlEventReceiver;
class ControlText;

class CameraRotateAction : public Component
{
private:
   CameraAction::Enum mAction;
   ControlEventReceiver* mEventRcvr;
   ControlText* mText;

   bool mLastDoubleTouch;
   bool mTouchDistanceThresholdReached;
   float mStartDoubleTouchDistance;
   float mLastDoubleTouchDistance;

public:
   CameraRotateAction(CameraAction::Enum action);

   virtual void Initialise(TickParameters& tp, GameObject* owner);
   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp) {}
};
