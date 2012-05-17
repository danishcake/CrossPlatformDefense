#include "Game.h"
#include <GLES2/gl2.h>
#include "Log.h"
#include "CheckMacros.h"
#include "Camera.h"
#include "Components/Graphics/Effect.h"
#include "Components/Graphics/Font.h"
#include "Components/Graphics/Texture.h"
#include "Components/Graphics/FadeOverlay.h"
#include "Components/States/MenuGS.h"
#include "Components/GUI/ControlEventDistributor.h"

#ifdef ANDROID
#include <android_native_app_glue.h>
#endif
#ifdef _WIN32
extern HWND create_window(int uiWidth, int uiHeight);
#endif
#ifdef __QNX__
#include <screen/screen.h>
#include <cstdlib>
#endif

#ifdef ANDROID
Game::Game(TickParameters* tp, android_app* app)
#endif
#ifdef _WIN32
Game::Game(TickParameters* tp)
#endif
#ifdef __QNX__
Game::Game(TickParameters* tp, screen_context_t ctx)
#endif
{
   mDisplay.Drawing = false;
   mDisplay.Width = 1024;
   mDisplay.Height = 600;
#ifdef ANDROID
   mDisplay.app = app;
#endif
#ifdef __QNX__
   mDisplay.qnx_screen_context = ctx;
#endif
   mTP = tp;

   //Spawn GUI service object
   GameObject* gui_service = new GameObject();
   gui_service->AddComponent(new ControlEventDistributor(), *mTP);
   mTP->Spawn(gui_service);

   GameObject* state = new GameObject();
   state->AddComponent(new MenuGS(), *mTP);
   mTP->Spawn(state);

   GameObject* fade_in = new GameObject();
   fade_in->AddComponent(new FadeOverlay(FadeType::FadeIn, Vector3f(1.0f, 1.0f, 1.0f), 0.5f), *mTP);
   mTP->Spawn(fade_in);
}

Game::~Game()
{
   for(std::vector<GameObject*>::iterator it = mGO.begin(); it != mGO.end(); ++it)
   {
      (*it)->TeardownGraphics(*mTP);
      (*it)->Teardown(*mTP);
      delete *it;
   }
}

void Game::Tick()
{
   mTP->draw_list.ClearAll();

   std::vector<GameObject*>& spawned = mTP->GetSpawned();
   for(std::vector<GameObject*>::iterator it = spawned.begin(); it != spawned.end(); ++it)
   {
      (*it)->Initialise(*mTP);
      (*it)->InitialiseGraphics(*mTP);
   }
   mGO.insert(mGO.end(), spawned.begin(), spawned.end());
   mTP->ClearSpawned();

   //Distribute messages
   mTP->msg.Distribute();


   //Tick objects
   for(std::vector<GameObject*>::iterator it = mGO.begin(); it != mGO.end(); ++it)
   {
      (*it)->Tick(*mTP);
   }

   //Remove and Teardown any that died this pass

   for(std::vector<GameObject*>::iterator it = mGO.begin(); it != mGO.end(); ++it)
   {
      if(GameObject::IsDead(*it))
      {
         (*it)->TeardownGraphics(*mTP);
         (*it)->Teardown(*mTP);
      }
   }

   mGO.erase(std::remove_if(mGO.begin(), mGO.end(), GameObject::IsDead), mGO.end());

   mTP->camera->Tick(*mTP);

   Texture::CacheTick(*mTP, TextureClearLevel::ClearUnused);
   Effect::CacheTick(*mTP, EffectClearLevel::ClearUnused);
   Font::CacheTick(*mTP, FontClearLevel::ClearUnused);
}

void Game::DrawFrame()
{
   if (mDisplay.Display == EGL_NO_DISPLAY) {
      // No display, just return
      return;
   }

   // Just fill the screen with a color.
   GL_CHECK(glClearColor(1, 0, 0, 1));
   GL_CHECK(glClearDepthf(1.00f));
   GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

   mTP->draw_list.DrawLayer(DrawLayer::Game, *mTP);
   mTP->draw_list.DrawLayer(DrawLayer::GUI, *mTP);
   mTP->draw_list.DrawLayer(DrawLayer::Fade, *mTP);

   eglSwapBuffers(mDisplay.Display, mDisplay.Surface);
}

void Game::TeardownDisplay()
{
   if (mDisplay.Display != EGL_NO_DISPLAY)
   {
      eglMakeCurrent(mDisplay.Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
      if (mDisplay.Context != EGL_NO_CONTEXT)
      {
         eglDestroyContext(mDisplay.Display, mDisplay.Context);
      }
      if (mDisplay.Surface != EGL_NO_SURFACE)
      {
         eglDestroySurface(mDisplay.Display, mDisplay.Surface);
      }
      eglTerminate(mDisplay.Display);
   }

   mDisplay.Display = EGL_NO_DISPLAY;
   mDisplay.Context = EGL_NO_CONTEXT;
   mDisplay.Surface = EGL_NO_SURFACE;

   //Teardown graphics
   Effect::CacheTick(*mTP, EffectClearLevel::Teardown);
   Texture::CacheTick(*mTP, TextureClearLevel::Teardown);
   Font::CacheTick(*mTP, FontClearLevel::Teardown);
   for(std::vector<GameObject*>::iterator it = mGO.begin(); it != mGO.end(); ++it)
   {
      (*it)->TeardownGraphics(*mTP);
   }

#ifdef __QNX__
   screen_destroy_context(mDisplay.qnx_screen_context);
#endif
}

bool Game::InitDisplay()
{
   /*
   * Here specify the attributes of the desired configuration.
   * Below, we select an EGLConfig with at least 8 bits per color
   * component compatible with on-screen windows
   */
   const EGLint attribs[] =
   {
      EGL_RED_SIZE,        8,
      EGL_GREEN_SIZE,      8,
      EGL_BLUE_SIZE,       8,
      EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
      EGL_DEPTH_SIZE,      24,
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, //Maybe EGL_CONFORMANT for windows and Android?
      EGL_NONE
   };
   EGLint attrib_list[]= { EGL_RED_SIZE,        8,
                           EGL_GREEN_SIZE,      8,
                           EGL_BLUE_SIZE,       8,
                           EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
                           EGL_RENDERABLE_TYPE, 0,
                           EGL_NONE};

   EGLint w, h, format;
   EGLint numConfigs;
   EGLConfig config[64];
   EGLSurface surface;
   EGLContext context;

   EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

   int maj;
   int min;
   eglInitialize(display, &maj, &min);
   Log::Debug("EGLInitialise", "EGL Major:%d Minor:%d", maj, min);

   /* Here, the application chooses the configuration it desires. In this
   * sample, we have a very simplified selection process, where we pick
   * the first EGLConfig that matches our criteria */
   eglChooseConfig(display, attribs, &config[0], 64, &numConfigs);

   for(int i = 0; i < numConfigs; i++)
   {
      int val[1];
      eglGetConfigAttrib(display, config[i], EGL_CONFORMANT, &val[0]);
      Log::Debug("EGLInitialise", "EGL_CONFORMANT: %d", val[0]);
      Log::Debug("EGLInitialise", "GL2: %d", val[0] & EGL_OPENGL_ES2_BIT);
      Log::Debug("EGLInitialise", "GL1: %d", val[0] & EGL_OPENGL_ES_BIT);
   }

   if(numConfigs == 0)
   {
      Log::Error("EGLInitialise", "No EGL configs were returned.");
      return true;
   }

   /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
   * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
   * As soon as we picked a EGLConfig, we can safely reconfigure the
   * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
   eglGetConfigAttrib(display, config[0], EGL_NATIVE_VISUAL_ID, &format);

#ifdef ANDROID
   ANativeWindow_setBuffersGeometry(mDisplay.app->window, 0, 0, format); //TODO!
   surface = eglCreateWindowSurface(display, config[0], mDisplay.app->window, NULL);
#endif
#ifdef _WIN32
   mDisplay.WindowHandle = create_window(mDisplay.Width, mDisplay.Height);
   surface = EGL_CHECK(eglCreateWindowSurface(display, config[0], (EGLNativeWindowType)mDisplay.WindowHandle, NULL));
#endif
#ifdef __QNX__
   int screen_format = SCREEN_FORMAT_RGBX8888;
   int usage = SCREEN_USAGE_OPENGL_ES2 | SCREEN_USAGE_ROTATION;
   int screen_resolution[2];
   screen_display_mode_t screen_mode;
   int size[2];
   int angle = std::atoi(std::getenv("ORIENTATION"));

   screen_create_window(&mDisplay.qnx_screen_win, mDisplay.qnx_screen_context);
   screen_set_window_property_iv(mDisplay.qnx_screen_win, SCREEN_PROPERTY_FORMAT, &screen_format);
   screen_set_window_property_iv(mDisplay.qnx_screen_win, SCREEN_PROPERTY_USAGE, &usage);
   screen_get_window_property_pv(mDisplay.qnx_screen_win, SCREEN_PROPERTY_DISPLAY, (void **)&mDisplay.qnx_screen_disp);
   screen_get_display_property_iv(mDisplay.qnx_screen_disp, SCREEN_PROPERTY_SIZE, screen_resolution);
   screen_get_display_property_pv(mDisplay.qnx_screen_disp, SCREEN_PROPERTY_MODE, (void**)&screen_mode);
   screen_get_window_property_iv(mDisplay.qnx_screen_win, SCREEN_PROPERTY_BUFFER_SIZE, size);

   int buffer_size[2] = {size[0], size[1]};

   if ((angle == 0) || (angle == 180)) {
       if (((screen_mode.width > screen_mode.height) && (size[0] < size[1])) ||
           ((screen_mode.width < screen_mode.height) && (size[0] > size[1]))) {
               buffer_size[1] = size[0];
               buffer_size[0] = size[1];
       }
   } else if ((angle == 90) || (angle == 270)){
       if (((screen_mode.width > screen_mode.height) && (size[0] > size[1])) ||
           ((screen_mode.width < screen_mode.height && size[0] < size[1]))) {
               buffer_size[1] = size[0];
               buffer_size[0] = size[1];
       }
   } else {
        return true;
   }

   screen_set_window_property_iv(mDisplay.qnx_screen_win, SCREEN_PROPERTY_BUFFER_SIZE, buffer_size);
   screen_set_window_property_iv(mDisplay.qnx_screen_win, SCREEN_PROPERTY_ROTATION, &angle);
   screen_create_window_buffers(mDisplay.qnx_screen_win, 2);

   surface = EGL_CHECK(eglCreateWindowSurface(display, config[0], (EGLNativeWindowType)mDisplay.qnx_screen_win, NULL));
#endif

   if(surface == EGL_NO_SURFACE) {
      Log::Error("EGLInitialise", "EGL Surface creation failed.");
      return true;
   }

   const EGLint attribs2[] =
   {
      EGL_CONTEXT_CLIENT_VERSION, 2,
      EGL_NONE
   };
   context = eglCreateContext(display, config[0], NULL, attribs2);

   if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
      Log::Error("EGLInitialise", "Unable to eglMakeCurrent");
      return true;
   }

#ifdef __QNX__
   eglSwapInterval(display, 1);
#endif

   eglQuerySurface(display, surface, EGL_WIDTH, &w);
   eglQuerySurface(display, surface, EGL_HEIGHT, &h);

   mDisplay.Display = display;
   mDisplay.Context = context;
   mDisplay.Surface = surface;
   mDisplay.Width = w;
   mDisplay.Height = h;
   mTP->camera->SetResolution(Vector2f((float)mDisplay.Width, (float)mDisplay.Height));

   // Initialize GL state.
   GL_CHECK(glViewport(0, 0, w, h));
   GL_CHECK(glEnable(GL_DEPTH_TEST));
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   GL_CHECK(glEnable(GL_BLEND));
   GL_CHECK(glEnable(GL_ALPHA));


   Effect::CacheTick(*mTP, EffectClearLevel::Initialise);
   Texture::CacheTick(*mTP, TextureClearLevel::Initialise);
   Font::CacheTick(*mTP, FontClearLevel::Initialise);
   //Initialise graphics
   for(std::vector<GameObject*>::iterator it = mGO.begin(); it != mGO.end(); ++it)
   {
      (*it)->InitialiseGraphics(*mTP);
   }

   return false;
}

void Game::SetDrawing(const bool drawing)
{
   if(mDisplay.Context != EGL_NO_CONTEXT && mDisplay.Surface != EGL_NO_SURFACE)
   {
      mDisplay.Drawing = drawing;
   } else
   {
      mDisplay.Drawing = false;
   }
}

const bool Game::GetDrawing()
{
   return mDisplay.Drawing;
}
