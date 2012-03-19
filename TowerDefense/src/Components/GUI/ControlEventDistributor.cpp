#include "ControlEventDistributor.h"
#include "ControlEventReceiver.h"
#include "../../GameObject.h"
#include "../../Log.h"

ControlEventDistributor::ControlEventDistributor()
{
}

void ControlEventDistributor::Initialise(TickParameters& tp, GameObject* owner)
{
   Component::Initialise(tp, owner);
}

void ControlEventDistributor::Tick(TickParameters& tp)
{
   std::vector<GameObject*> receivers = tp.memberships.GetMembers(MembershipType::Gui);

   for(std::vector<GameObject*>::iterator it = receivers.begin(); it != receivers.end(); ++it)
   {
      ControlEventReceiver* control_rcvr = (*it)->GetComponent<ControlEventReceiver>();
      control_rcvr->ClearEvents();


      if(tp.input_pressed.count(InputAction::Click))
      {
         int x = tp.input_pressed[InputAction::Click].data.ClickData.ex;
         int y = tp.input_pressed[InputAction::Click].data.ClickData.ey;
         if(control_rcvr->WithinArea(tp, x, y))
            control_rcvr->SetClicked(x, y);
      }

      if(tp.input_held.count(InputAction::HoldMove))
      {
         int x = tp.input_held[InputAction::HoldMove].data.HoldMoveData.x;
         int y = tp.input_held[InputAction::HoldMove].data.HoldMoveData.y;
         int dx = tp.input_held[InputAction::HoldMove].data.HoldMoveData.dx;
         int dy = tp.input_held[InputAction::HoldMove].data.HoldMoveData.dy;
         int sx = tp.input_held[InputAction::HoldMove].data.HoldMoveData.sx;
         int sy = tp.input_held[InputAction::HoldMove].data.HoldMoveData.sy;
         if(control_rcvr->WithinArea(tp, sx, sy))
         {
            control_rcvr->SetDrag(x, y, sx, sy, dx, dy);
         }
      }

   }
}

void ControlEventDistributor::Teardown(TickParameters& tp)
{
}
