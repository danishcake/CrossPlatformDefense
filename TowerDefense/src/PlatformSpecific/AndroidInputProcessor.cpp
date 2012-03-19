#include "AndroidInputProcessor.h"
#include "../Log.h"

AndroidInputProcessor::AndroidInputProcessor()
	: mScreenHeld(false), mScreenStartX(0), mScreenStartY(0)
{

}

void AndroidInputProcessor::GetActions(ActionMap& held, ActionMap& pressed, ActionMap& released)
{
	//Polling here
	//Calculate deltas
	IInputProcessor::GetActions(held, pressed, released);
}

bool AndroidInputProcessor::Process(AInputEvent* event)
{
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        InputAction tap;
        tap.action = InputAction::Tap;
        tap.data.TapData.x = AMotionEvent_getX(event, 0);
        tap.data.TapData.y = AMotionEvent_getY(event, 0);
        mActions[InputAction::Tap] = tap;
        return true;
    }

    return false;
}
