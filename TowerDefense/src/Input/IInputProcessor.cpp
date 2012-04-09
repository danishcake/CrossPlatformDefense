#include "IInputProcessor.h"
#include "../Log.h"
#include <cstdlib>
#include <cassert>


static int LargestOf(const int a, const int b)
{
   if(std::abs(a) > std::abs(b))
      return std::abs(a);
   else
      return std::abs(b);
}

void IInputProcessor::GetActions(ActionMap& held, ActionMap& pressed, ActionMap& released)
{
   if (mCurrentTouch.GetTouchCount() > 0)
   {
      InputAction move;
      move.action = InputAction::HoldMove;
      move.data.HoldMoveData.current = mCurrentTouch;
      move.data.HoldMoveData.start = mCurrentTouch;
      move.data.HoldMoveData.prev = mCurrentTouch;

      if (mActionsLastTime.count(InputAction::HoldMove))
      {
         move.data.HoldMoveData.start = mActionsLastTime[InputAction::HoldMove].data.HoldMoveData.start;
         move.data.HoldMoveData.prev = mActionsLastTime[InputAction::HoldMove].data.HoldMoveData.current;

         // When number of touch points changes reset the move event
         if (mCurrentTouch.GetTouchCount() != mActionsLastTime[InputAction::HoldMove].data.HoldMoveData.start.GetTouchCount())
         {
            move.data.HoldMoveData.start = move.data.HoldMoveData.current;
         }
      }

      mActions[InputAction::HoldMove] = move;
   }

   // Set a click event if touch events all in same position
   if (!mActions.count(InputAction::HoldMove) && mActionsLastTime.count(InputAction::HoldMove))
   {
      TouchData td_current = mActionsLastTime[InputAction::HoldMove].data.HoldMoveData.current;
      TouchData td_start = mActionsLastTime[InputAction::HoldMove].data.HoldMoveData.start;
      if (td_current.GetTouchCount() == 1)
      {
         Vector2i delta = td_current.GetTouch(0) - td_start.GetTouch(0);
         if (delta.lengthSq() < 100)
         {
            InputAction click;
            click.action = InputAction::Click;
            click.data.ClickData.end = mActionsLastTime[InputAction::HoldMove].data.HoldMoveData.current;
            mActions[InputAction::Click] = click;
         }
      }
   }

   held = mActions;
   pressed.clear();
   released.clear();


   for(std::map<InputAction::Enum, InputAction>::iterator now_it = mActions.begin(); now_it != mActions.end(); ++now_it)
   {
      if(mActionsLastTime.count(now_it->first) == 0)
      {
         pressed[now_it->first] = now_it->second;
      }
   }

   for(std::map<InputAction::Enum, InputAction>::iterator last_it = mActionsLastTime.begin(); last_it != mActionsLastTime.end(); ++last_it)
   {
      if(mActions.count(last_it->first) == 0)
      {
         released[last_it->first] = last_it->second;
      }
   }
}

void IInputProcessor::ClearActions()
{
   mActionsLastTime = mActions;
   mActions.clear();
   mCurrentTouch = TouchData();
}
