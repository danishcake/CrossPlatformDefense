#pragma once
#include "../Component.h"

class ControlArea;

/*
 * ControlEventReceiver contains code to check if events occur within the control
 * bounds defined by a ControlArea. It also keeps records of the events that occur
 * to it.
 * If no ControlArea is present then events will not be set.
 */
class ControlEventReceiver : public Component
{
private:
   ControlArea* mControlArea;
   bool mClicked;
   TouchData mClick;

   bool mDrag;
   TouchData mDragStart;
   TouchData mDragPrev;
   TouchData mDragCurrent;

public:
   ControlEventReceiver();

   virtual void Initialise(TickParameters& tp, GameObject* owner);
   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp);

   void ClearEvents();
   bool WithinArea(TickParameters& tp, int x, int y);

   void SetClicked(TouchData td);

   bool GetClicked();
   TouchData GetClick();

   void SetDrag(TouchData start, TouchData prev, TouchData current);

   bool GetDrag();
   TouchData GetDragStart();
   TouchData GetDragPrev();
   TouchData GetDragCurrent();
};
