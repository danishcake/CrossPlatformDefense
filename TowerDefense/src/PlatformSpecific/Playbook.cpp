/*
 * This is largely lifted HelloBlackberry example
 */
#include <assert.h>
#include <screen/screen.h>
#include <bps/navigator.h>
#include <bps/screen.h>
#include <bps/bps.h>
#include <bps/event.h>

#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <cmath>



#include "../Log.h"
#include "../CheckMacros.h"
#include "../Game.h"
#include "../TickParameters.h"
#include "PlaybookFile.h"
#include "PlaybookInputProcessor.h"


static PlaybookInputProcessor input_processor;
static screen_context_t screen_cxt;


void handleScreenEvent(bps_event_t *event) {
   screen_event_t screen_event = screen_event_get_event(event);

   int screen_val;
   screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TYPE, &screen_val);

   switch (screen_val) {
   case SCREEN_EVENT_MTOUCH_TOUCH:
   case SCREEN_EVENT_MTOUCH_MOVE:
   case SCREEN_EVENT_MTOUCH_RELEASE:
      break;
   }
}

int main(int argc, const char* argv[])
{
   bool bDone = false;

   //Create a screen context that will be used to create an EGL surface to to receive libscreen events
   screen_create_context(&screen_cxt, 0);

   //Initialize BPS library
   bps_initialize();


   TickParameters* tp = new TickParameters(new PlaybookFile());
   Game* game = new Game(tp, screen_cxt);
   bDone = game->InitDisplay();
   
   //Signal BPS library that navigator and screen events will be requested
   if (BPS_SUCCESS != screen_request_events(screen_cxt))
   {
      fprintf(stderr, "screen_request_events failed\n");
      screen_destroy_context(screen_cxt);
      bps_shutdown();
      return 0;
   }

   if (BPS_SUCCESS != navigator_request_events(0)) {
      fprintf(stderr, "navigator_request_events failed\n");
      screen_destroy_context(screen_cxt);
      bps_shutdown();
      return 0;
   }

   /* Enter event loop */
   while (!bDone)
   {
      input_processor.ClearActions();
        //Request and process all available BPS events
        bps_event_t *event = NULL;

      while(true)
      {
         int rc = bps_get_event(&event, 0);
         assert(rc == BPS_SUCCESS);

         if (event)
         {
            if(!input_processor.Process(event))
            {
               int domain = bps_event_get_domain(event);

               if(domain == navigator_get_domain() &&
                  NAVIGATOR_EXIT == bps_event_get_code(event))
               {
                  bDone = true;
               }
            }
         } else
         {
            break;
         }
      }
      input_processor.GetActions(tp->input_held, tp->input_pressed, tp->input_released);
      game->Tick();
      game->DrawFrame();

      usleep(10);
   }

   game->TeardownDisplay();
   delete game;
   delete tp;
}
