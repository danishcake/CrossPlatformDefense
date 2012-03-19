#include <vector>
#include <EGL/egl.h>
#include "GameObject.h"
#include "WorldBlocks.h"

struct TickParameters;

#ifdef ANDROID
struct android_app;
#endif

#ifdef __QNX__
struct _screen_context;
typedef struct _screen_context* screen_context_t;
struct _screen_window;
typedef struct _screen_window* screen_window_t;
struct _screen_display;
typedef struct _screen_display* screen_display_t;
#endif

class Game
{
private:
   struct
   {
      EGLDisplay Display;
      EGLSurface Surface;
      EGLContext Context;
      bool Drawing;
      int Width;
      int Height;
#ifdef ANDROID
     android_app* app;
#endif
#ifdef _WIN32
     HWND WindowHandle;
#endif
#ifdef __QNX__
     screen_context_t qnx_screen_context;
     screen_window_t qnx_screen_win;
     screen_display_t qnx_screen_disp;
#endif
   } mDisplay;

   TickParameters* mTP;
   std::vector<GameObject*> mGO;
public:
#ifdef ANDROID
   Game(TickParameters* tp, android_app* app);
#endif
#ifdef _WIN32
   Game(TickParameters* tp);
#endif
#ifdef __QNX__
   Game(TickParameters* tp, screen_context_t ctx);
#endif

   ~Game();
   void Tick();
   void DrawFrame();

   void TeardownDisplay();
   /*
    * InitDisplay returns true if an error occurred
    */
   bool InitDisplay();
   void SetDrawing(const bool drawing);
   const bool GetDrawing();
};
