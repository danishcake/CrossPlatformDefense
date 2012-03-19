#include "PlaybookInputProcessor.h"
#include <bps/screen.h>

PlaybookInputProcessor::PlaybookInputProcessor() :
   mDown(false)
{
}

void PlaybookInputProcessor::GetActions(ActionMap& held, ActionMap& pressed, ActionMap& released)
{
   // Tap drag calculations
   if(mDown)
   {
      InputAction action;
      action.action = InputAction::Tap;
      action.data.TapData.x = mXY[0];
      action.data.TapData.y = mXY[1];
      mActions[InputAction::Tap] = action;
   }

   //Calculate deltas
   IInputProcessor::GetActions(held, pressed, released);
}

bool PlaybookInputProcessor::Process(bps_event_t* event)
{
   bool handled = false;
   int domain = bps_event_get_domain(event);

   if(domain == screen_get_domain())
   {
      screen_event_t screen_event = screen_event_get_event(event);

      int screen_val;
      screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TYPE, &screen_val);

      switch (screen_val) {
      case SCREEN_EVENT_MTOUCH_TOUCH:
         screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_SOURCE_POSITION, mXY);
         mDown = true;
         handled = true;

         break;
      case SCREEN_EVENT_MTOUCH_MOVE:
         screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_SOURCE_POSITION, mXY);
         mDown = true;
         handled = true;
         break;
      case SCREEN_EVENT_MTOUCH_RELEASE:
         screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_SOURCE_POSITION, mXY);
         mDown = false;
         handled = true;
         break;
      default: //Other events are not handled
         break;
      }
   }

   return handled;
}
