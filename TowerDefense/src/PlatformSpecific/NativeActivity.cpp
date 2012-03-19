#include <jni.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <vector>
#include "../WorldBlocks.h"
#include "../TickParameters.h"
#include "../GameObject.h"

#include "../Game.h"
#include "../TickParameters.h"
#include "../Log.h"
#include "AndroidFile.h"
#include "AndroidInputProcessor.h"


struct Engine {
    struct android_app* app;
    Game* game;
	AndroidInputProcessor* input_processor;
	TickParameters* tp;

    Engine(android_app* app)
    : app(app)
    {
		tp = new TickParameters(new AndroidFile(app->activity->assetManager));
		game = new Game(tp, app);
		input_processor = new AndroidInputProcessor();
    }
};

static void draw_frame(struct Engine* engine)
{
	engine->game->DrawFrame();
}

static void init_display(Engine* engine)
{
	engine->game->InitDisplay();
}

static void teardown_display(Engine* engine)
{
	engine->game->TeardownDisplay();
}

//Touch input
static int32_t handle_input(struct android_app* app, AInputEvent* event)
{
    struct Engine* engine = (struct Engine*)app->userData;
	return engine->input_processor->Process(event);
}

static void handle_cmd(struct android_app* app, int32_t cmd)
{
   struct Engine* engine = (struct Engine*)app->userData;

   switch (cmd)
   {
      case APP_CMD_SAVE_STATE:
         //Could save state here (and restore in android_main)
         break;
      case APP_CMD_INIT_WINDOW:
         //Create the opengl context
         if (engine->app->window != NULL)
         {
            init_display(engine);
            engine->game->SetDrawing(true);
         }
         break;
      case APP_CMD_TERM_WINDOW:
         //Teardown the opengl context
         teardown_display(engine);
         break;
      case APP_CMD_GAINED_FOCUS:
         //Start any power consuming monitoring
         //Start drawing again
    	  engine->game->SetDrawing(true);
         break;
      case APP_CMD_LOST_FOCUS:
         //Stop any power consuming monitoring
         //Stop drawing
    	  engine->game->SetDrawing(false);
         break;
   }
}

static void tick(Engine* engine)
{
   engine->input_processor->GetActions(engine->tp->input_held,
									   engine->tp->input_pressed,
									   engine->tp->input_released);
   engine->game->Tick();
}

/* Application entry point */
void android_main(struct android_app* state) {
   Engine engine(state);

   app_dummy();

   //Set callbacks for commands and input
   state->onAppCmd = handle_cmd;
   state->onInputEvent = handle_input;

   //Store my state userdata
   state->userData = &engine;

   //Run event loop
   while (1)
   {
      // Read all pending events.
      int ident;
      int events;
      struct android_poll_source* source;

      engine.input_processor->ClearActions();
      //Loop over events. Block if not drawing
      while ((ident=ALooper_pollAll(engine.game->GetDrawing() ? 0 : -1, NULL, &events, (void**)&source)) >= 0)
      {
         // Process this event.
         if (source != NULL) {
             source->process(state, source);
         }

         // Check if we are exiting.
         if (state->destroyRequested != 0) {
             teardown_display(&engine);
             return;
         }
      }

      if (engine.game->GetDrawing())
      {
         //Update simulation
         tick(&engine);
         // Drawing is throttled to the screen update rate, so there
         // is no need to do timing here.
         draw_frame(&engine);
      }
   }
}
