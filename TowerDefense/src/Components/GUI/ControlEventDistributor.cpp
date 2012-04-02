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
         Vector2i pos = tp.input_pressed[InputAction::Click].data.ClickData.end.GetTouchCentre();
         if(control_rcvr->WithinArea(tp, pos.x, pos.y))
            control_rcvr->SetClicked(pos.x, pos.y);
      }

      if(tp.input_held.count(InputAction::HoldMove))
      {
         // Two finger pan/zoom
         if (tp.input_held[InputAction::HoldMove].data.HoldMoveData.current.GetTouchCount() == 2)
         {
         }
         // One finger rotate
         if (tp.input_held[InputAction::HoldMove].data.HoldMoveData.current.GetTouchCount() == 1)
         {
            Vector2i c_pos = tp.input_held[InputAction::HoldMove].data.HoldMoveData.current.GetTouchCentre();
            Vector2i s_pos= tp.input_held[InputAction::HoldMove].data.HoldMoveData.start.GetTouchCentre();
            Vector2i p_pos = tp.input_held[InputAction::HoldMove].data.HoldMoveData.prev.GetTouchCentre();
            Vector2i d_pos = c_pos - p_pos;

            if(control_rcvr->WithinArea(tp, s_pos.x, s_pos.y))
            {
               control_rcvr->SetDrag(c_pos.x, c_pos.y, s_pos.x, s_pos.y, d_pos.x, d_pos.y);
            }
         }
      }

   }
}

void ControlEventDistributor::Teardown(TickParameters& tp)
{
}
