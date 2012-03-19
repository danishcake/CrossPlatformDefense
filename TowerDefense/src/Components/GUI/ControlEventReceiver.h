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
   int mClickX;
   int mClickY;

   bool mDrag;
   int mDragX;
   int mDragY;
   int mDragSX;
   int mDragSY;
   int mDragDX;
   int mDragDY;

public:
   ControlEventReceiver();

   virtual void Initialise(TickParameters& tp, GameObject* owner);
   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp);

   void ClearEvents();
   bool WithinArea(TickParameters& tp, int x, int y);

   void SetClicked(int x, int y);

   bool GetClicked();
   int GetClickX();
   int GetClickY();
   
   void SetDrag(int x, int y, int sx, int sy, int dx, int dy);

   bool GetDrag();
   int GetDragX();
   int GetDragY();
   int GetDragSX();
   int GetDragSY();
   int GetDragDX();
   int GetDragDY();
};
