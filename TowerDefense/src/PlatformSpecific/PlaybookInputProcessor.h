#pragma once
#include "../Input/IInputProcessor.h"
#include <bps/event.h>

/*
 * PlaybookInputProcessor tracks up to two touch positions
 */
class PlaybookInputProcessor : public IInputProcessor
{
private:
   Vector2i mTouchPosition[2];
   bool mDown[2];
   bool mSet[2];
public:
   PlaybookInputProcessor();
   virtual void GetActions(ActionMap& held, ActionMap& pressed, ActionMap& released);

   bool Process(bps_event_t* event);
};

