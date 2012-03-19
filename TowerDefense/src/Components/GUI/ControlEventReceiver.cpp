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
void ControlEventReceiver::SetClicked(int x, int y)
{
   mClicked = true;
   mClickX = x;
   mClickY = y;
}

bool ControlEventReceiver::GetClicked()
{
   return mClicked;
}

int ControlEventReceiver::GetClickX()
{
   return mClickX;
}

int ControlEventReceiver::GetClickY()
{
   return mClickY;
}


//Drag stuff
void ControlEventReceiver::SetDrag(int x, int y, int sx, int sy, int dx, int dy)
{
   mDrag = true;
   mDragX = x;
   mDragY = y;
   mDragSX = sx;
   mDragSY = sy;
   mDragDX = dx;
   mDragDY = dy;
}

bool ControlEventReceiver::GetDrag()
{
   return mDrag;
}

int ControlEventReceiver::GetDragX()
{
   return mDragX;
}

int ControlEventReceiver::GetDragY()
{
   return mDragY;
}

int ControlEventReceiver::GetDragSX()
{
   return mDragSX;
}

int ControlEventReceiver::GetDragSY()
{
   return mDragSY;
}

int ControlEventReceiver::GetDragDX()
{
   return mDragDX;
}

int ControlEventReceiver::GetDragDY()
{
   return mDragDY;
}


