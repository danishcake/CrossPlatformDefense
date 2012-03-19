#pragma once
#include "../../Component.h"
#include <boost/bind.hpp>
#include <boost/signals2.hpp>

class ControlEventReceiver;


/*
 * Generic listener for clicks that fires a callback.
 * Can be used to callback a particular object.
 */
class SignalAction : public Component
{
public:
   typedef boost::signals2::signal<void (int x, int y, TickParameters& tp)> OnClick;
   typedef OnClick::slot_type OnClickSlotType;

   SignalAction(const OnClickSlotType& slot);

   virtual void Initialise(TickParameters& tp, GameObject* owner);
   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp) {}

private:
   ControlEventReceiver* mEventRcvr;
   OnClick mOnClick;
};
