#pragma once
#include "../Input/IInputProcessor.h"
#include <bps/event.h>

class PlaybookInputProcessor : public IInputProcessor
{
private:
   bool mDown;
   int mXY[2];
public:
   PlaybookInputProcessor();
   virtual void GetActions(ActionMap& held, ActionMap& pressed, ActionMap& released);

   bool Process(bps_event_t* event);
};

