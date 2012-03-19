#include "IInputProcessor.h"
#include "../Log.h"
#include <cstdlib>

static int LargestOf(const int a, const int b)
{
   if(std::abs(a) > std::abs(b))
      return std::abs(a);
   else
      return std::abs(b);
}

static int LargestOf(const int a, const int b, const int c)
{
   return LargestOf(LargestOf(a, b), c);
}

void IInputProcessor::GetActions(ActionMap& held, ActionMap& pressed, ActionMap& released)
{
   if(mActions.count(InputAction::Tap))
   {
      if(mActionsLastTime.count(InputAction::HoldStart))
      { //Continue press (first pass)
         InputAction move;
         move.action = InputAction::HoldMove;
         move.data.HoldMoveData.x = mActions[InputAction::Tap].data.TapData.x;
         move.data.HoldMoveData.y = mActions[InputAction::Tap].data.TapData.y;
         move.data.HoldMoveData.sx = mActionsLastTime[InputAction::HoldStart].data.HoldStartData.x;
         move.data.HoldMoveData.sy = mActionsLastTime[InputAction::HoldStart].data.HoldStartData.y;
         move.data.HoldMoveData.dx = move.data.HoldMoveData.x - move.data.HoldMoveData.sx;
         move.data.HoldMoveData.dy = move.data.HoldMoveData.y - move.data.HoldMoveData.sy;
         move.data.HoldMoveData.max_displacement = LargestOf(move.data.HoldMoveData.dx, move.data.HoldMoveData.dy);
         mActions[InputAction::HoldMove] = move;
      } else if(mActionsLastTime.count(InputAction::HoldMove))
      { //Continue press
         InputAction move;
         move.action = InputAction::HoldMove;
         move.data.HoldMoveData.x = mActions[InputAction::Tap].data.TapData.x;
         move.data.HoldMoveData.y = mActions[InputAction::Tap].data.TapData.y;
         move.data.HoldMoveData.sx = mActionsLastTime[InputAction::HoldMove].data.HoldMoveData.sx;
         move.data.HoldMoveData.sy = mActionsLastTime[InputAction::HoldMove].data.HoldMoveData.sy;
         move.data.HoldMoveData.dx = move.data.HoldMoveData.x - mActionsLastTime[InputAction::HoldMove].data.HoldMoveData.x;
         move.data.HoldMoveData.dy = move.data.HoldMoveData.y - mActionsLastTime[InputAction::HoldMove].data.HoldMoveData.y;
         move.data.HoldMoveData.max_displacement = LargestOf(move.data.HoldMoveData.dx, move.data.HoldMoveData.dy,
                                                             mActionsLastTime[InputAction::HoldMove].data.HoldMoveData.max_displacement);
         mActions[InputAction::HoldMove] = move;
      } else //Start of press
      {
         InputAction start;
         start.action = InputAction::HoldStart;
         start.data.HoldStartData.x = mActions[InputAction::Tap].data.TapData.x;
         start.data.HoldStartData.y = mActions[InputAction::Tap].data.TapData.y;
         mActions[InputAction::HoldStart] = start;
      }
   } else
   {
      if(mActionsLastTime.count(InputAction::HoldMove))
      {
         InputAction end;
         end.action = InputAction::HoldEnd;
         end.data.HoldEndData.sx = mActionsLastTime[InputAction::HoldMove].data.HoldMoveData.sx;
         end.data.HoldEndData.sy = mActionsLastTime[InputAction::HoldMove].data.HoldMoveData.sy;
         end.data.HoldEndData.ex = mActionsLastTime[InputAction::HoldMove].data.HoldMoveData.x;
         end.data.HoldEndData.ey = mActionsLastTime[InputAction::HoldMove].data.HoldMoveData.y;
         end.data.HoldEndData.max_displacement = mActionsLastTime[InputAction::HoldMove].data.HoldMoveData.max_displacement;

         mActions[InputAction::HoldEnd] = end;
      } else if(mActionsLastTime.count(InputAction::HoldStart))
      {
         InputAction end;
         end.action = InputAction::HoldEnd;
         end.data.HoldEndData.sx = mActionsLastTime[InputAction::HoldStart].data.HoldStartData.x;
         end.data.HoldEndData.sy = mActionsLastTime[InputAction::HoldStart].data.HoldStartData.y;
         end.data.HoldEndData.ex = mActionsLastTime[InputAction::HoldStart].data.HoldStartData.x;
         end.data.HoldEndData.ey = mActionsLastTime[InputAction::HoldStart].data.HoldStartData.y;
         end.data.HoldEndData.max_displacement = 0;

         mActions[InputAction::HoldEnd] = end;
      }
   }

   // If tap never moves more than 10px from start
   if(mActions.count(InputAction::HoldEnd) && mActions[InputAction::HoldEnd].data.HoldEndData.max_displacement < 10)
   {
      InputAction click;
      click.action = InputAction::Click;
      click.data.ClickData.sx = mActions[InputAction::HoldEnd].data.HoldEndData.sx;
      click.data.ClickData.sy = mActions[InputAction::HoldEnd].data.HoldEndData.sy;
      click.data.ClickData.ex = mActions[InputAction::HoldEnd].data.HoldEndData.ex;
      click.data.ClickData.ey = mActions[InputAction::HoldEnd].data.HoldEndData.ey;
      click.data.ClickData.max_displacement = mActions[InputAction::HoldEnd].data.HoldEndData.max_displacement;

      mActions[InputAction::Click] = click;
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
}
