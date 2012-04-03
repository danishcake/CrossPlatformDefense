#include "ControlEventReceiver.h"
#include "ControlArea.h"
#include "../../GameObject.h"
#include "../../Camera.h"

ControlEventReceiver::ControlEventReceiver()
   :mControlArea(0),
    mClicked(false),
    mDrag(false)
{
}

void ControlEventReceiver::Initialise(TickParameters& tp, GameObject* owner)
{
   Component::Initialise(tp, owner);
   mControlArea = owner->GetComponent<ControlArea>();
   tp.memberships.Register(MembershipType::Gui, owner);
}

void ControlEventReceiver::Teardown(TickParameters& tp)
{
   tp.memberships.Unregister(MembershipType::Gui, mOwner);
}

void ControlEventReceiver::Tick(TickParameters& tp)
{
}

void ControlEventReceiver::ClearEvents()
{
   mClicked = false;
   mDrag = false;
}

bool ControlEventReceiver::WithinArea(TickParameters& tp, int x, int y)
{
   Vector2f control_size = mControlArea->GetSize().ToScreen(tp.camera->GetResolution());
   Vector2f top_left = mControlArea->GetPosition().ToScreen(tp.camera->GetResolution(), control_size);
   Vector2f bottom_right = top_left + control_size;

   return (x >= top_left.x && x < bottom_right.x && y >= top_left.y && y < bottom_right.y);
}


//Click stuff
void ControlEventReceiver::SetClicked(TouchData td)
{
   mClicked = true;
   mClick = td;
}

bool ControlEventReceiver::GetClicked()
{
   return mClicked;
}

TouchData ControlEventReceiver::GetClick()
{
   return mClick;
}


//Drag stuff
void ControlEventReceiver::SetDrag(TouchData start, TouchData prev, TouchData current)
{
   mDrag = true;
   mDragStart = start;
   mDragPrev = prev;
   mDragCurrent = current;
}

bool ControlEventReceiver::GetDrag()
{
   return mDrag;
}

TouchData ControlEventReceiver::GetDragStart()
{
   return mDragStart;
}

TouchData ControlEventReceiver::GetDragPrev()
{
   return mDragPrev;
}

TouchData ControlEventReceiver::GetDragCurrent()
{
   return mDragCurrent;
}


