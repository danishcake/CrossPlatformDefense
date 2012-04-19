#pragma once

#include "../Component.h"
#include "../../Messaging/GuiTransitionControlMessage.h"

class ControlArea;
class ControlOutline;
class ControlText;
class ControlProgress;

namespace ControlTransitionState
{
   enum Enum
   {
      TransIn,    // Becoming more visible
      TransOut,   // Becoming less visible
      Visible,    // 100% size, 100% alpha
      Hidden      // 70% size, 0% alpha
   };
}

/*
 * ControlTransition handles the introduction of the control
 * It has four states:
 * Transitioning in
 * Transitioning out
 * Visible
 * Hidden
 * Outline and text are scaled and faded during the transition
 */
class ControlTransition : public Component
{
private:
   ControlArea* mControlArea;
   ControlOutline* mControlOutline;
   ControlText* mControlText;
   ControlProgress* mControlProgress;

   Subscriber<GuiTransitionControlMessage> mTransitionControlSubscriber;

   float mTransitionTimer;
   float mTransitionPeriod;
   ControlTransitionState::Enum mPendingState;

   int mTransitionID;
public:
   ControlTransition(ControlTransitionState::Enum state, float period = 0.25f, int control_transition_group = -1);

   virtual void Initialise(TickParameters& tp, GameObject* owner);
   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp) {};
};
