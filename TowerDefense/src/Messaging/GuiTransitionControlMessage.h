#pragma once

namespace GuiTransition
{
   enum Enum
   {
      TransitionIn,
      TransitionOut
   };
}

/*
 * This message is intended to cause ControlTransitions to change state
 */
class GuiTransitionControlMessage
{
public:
   int TransitionID;
   GuiTransition::Enum TransitionType;

   GuiTransitionControlMessage(int transition_id, GuiTransition::Enum transition_type)
   {
      TransitionID = transition_id;
      TransitionType = transition_type;
   }
};
