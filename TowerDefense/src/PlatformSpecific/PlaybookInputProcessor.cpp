#include "PlaybookInputProcessor.h"
#include <bps/screen.h>
#include "../Log.h"

PlaybookInputProcessor::PlaybookInputProcessor()
{
   for(int i = 0; i < 2; i++)
   {
      mDown[i] = false;
      mSet[i] = false;
   }
}

void PlaybookInputProcessor::GetActions(ActionMap& held, ActionMap& pressed, ActionMap& released)
{
   int max_touch_count = mCurrentTouch.GetMaxTouchCount();
   mCurrentTouch = TouchData();
   // Tap drag calculations
   for(int i = 0; i < 2; i++)
   {
      if(mDown[i])
      {
         mCurrentTouch.AddTouch(mTouchPosition[i]);
      }
      mSet[i] = false;
   }
   if(mCurrentTouch.GetTouchCount() == 1)
      Log::Debug(__FILE__, "Single touch at (%d, %d)", mCurrentTouch.GetTouch(0).x, mCurrentTouch.GetTouch(0).y);
   if(mCurrentTouch.GetTouchCount() == 2)
      Log::Debug(__FILE__, "Double touch at (%d, %d), (%d, %d) centre (%d, %d)", mCurrentTouch.GetTouch(0).x, mCurrentTouch.GetTouch(0).y,
            mCurrentTouch.GetTouch(1).x, mCurrentTouch.GetTouch(1).y, mCurrentTouch.GetTouchCentre().x, mCurrentTouch.GetTouchCentre().y);

   // If no touches then clear touch, otherwise propogate the max to the next frame
   if(mCurrentTouch.GetTouchCount() == 0)
      max_touch_count = 0;

   mCurrentTouch.SetMaxTouchCount(max_touch_count);

   //Calculate deltas
   IInputProcessor::GetActions(held, pressed, released);
}

bool PlaybookInputProcessor::Process(bps_event_t* event)
{
   bool handled = false;
   Vector2i position;
   int domain = bps_event_get_domain(event);

   if(domain == screen_get_domain())
   {
      screen_event_t screen_event = screen_event_get_event(event);

      int screen_val;
      int touch_id;
      screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TYPE, &screen_val);

      switch (screen_val) {
      case SCREEN_EVENT_MTOUCH_TOUCH:
      case SCREEN_EVENT_MTOUCH_MOVE:
         screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TOUCH_ID, &touch_id);
         screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_SOURCE_POSITION, &position[0]);

         if(touch_id < 2 && !mSet[touch_id])
         {
            //Log::Debug(__FILE__, "Using touch index %d at (%d, %d)", touch_id, position.x, position.y);
            mDown[touch_id] = true;
            mTouchPosition[touch_id] = position;
            mSet[touch_id] = true;
         } else
         {
            //Log::Debug(__FILE__, "Ignoring touch index %d at (%d, %d)", touch_id, position.x, position.y);
         }
         handled = true;
         break;
      case SCREEN_EVENT_MTOUCH_RELEASE:
         screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TOUCH_ID, &touch_id);
         screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_SOURCE_POSITION, &position[0]);
         if(touch_id < 2)
         {
            Log::Debug(__FILE__, "Using touchend index %d at (%d, %d)", touch_id, position.x, position.y);
            mDown[touch_id] = false;
            mTouchPosition[touch_id] = position;
            mSet[touch_id] = true;
         } else
         {
            Log::Debug(__FILE__, "Ignoring touchend index %d at (%d, %d)", touch_id, position.x, position.y);
         }
         handled = true;
         break;
      default: //Other events are not handled
         break;
      }
   }

   return handled;
}
