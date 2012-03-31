/*
 * This is largely lifted from ARM OpenGL ES 2.0 emulator examples
 */
#define WIN32_LEAN_AND_MEAN 1
#define NOMINMAX
#include <windows.h>
#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "../Log.h"
#include "../CheckMacros.h"
#include "../Game.h"
#include "../TickParameters.h"
#include "WindowsFile.h"
#include "WindowsInputProcessor.h"


static WindowsInputProcessor input_processor;

LRESULT CALLBACK process_window(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

/* 
 * create_window(): Set up Windows specific bits.
 *
 * uiWidth:    Width of window to create.
 * uiHeight:   Height of window to create.
 *
 * Returns:    Device specific window handle.
 */
HWND create_window(int uiWidth, int uiHeight) {
   WNDCLASS wc;
   RECT wRect;
   HWND sWindow;
   HINSTANCE hInstance;

   wRect.left = 0L;
   wRect.right = (long)uiWidth;
   wRect.top = 0L;
   wRect.bottom = (long)uiHeight;

   hInstance = GetModuleHandle(NULL);

   wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
   wc.lpfnWndProc = (WNDPROC)process_window;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = 0;
   wc.hInstance = hInstance;
   wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
   wc.hCursor = LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground = NULL;
   wc.lpszMenuName = NULL;
   wc.lpszClassName = L"OGLES";

   RegisterClass(&wc);

   AdjustWindowRectEx(&wRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);

   sWindow = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, L"OGLES", L"main", WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, uiWidth, uiHeight, NULL, NULL, hInstance, NULL);

   ShowWindow(sWindow, SW_SHOW);
   SetForegroundWindow(sWindow);
   SetFocus(sWindow);

   return sWindow;
}

/*
 * process_window(): This function handles Windows callbacks.
 */
LRESULT CALLBACK process_window(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) {
   switch(uiMsg) {
      case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
      case WM_SIZE:
      case WM_ACTIVATE:
         return 0;
      default:
         if(input_processor.Process(hWnd, uiMsg, wParam, lParam))
            return 0;
         break;
   }

   return DefWindowProc(hWnd, uiMsg, wParam, lParam);
}

int main(int argc, const char* argv[])
{
   bool bDone = false;
   MSG sMessage;

   TickParameters* tp = new TickParameters(new WindowsFile());
   Game* game = new Game(tp);
   bDone = game->InitDisplay();

   /* Enter event loop */
    while (!bDone)
   {
      input_processor.ClearActions();
        if(PeekMessage(&sMessage, NULL, 0, 0, PM_REMOVE)) {
            if(sMessage.message == WM_QUIT) {
                bDone = 1;
            } else {
                TranslateMessage(&sMessage);
                DispatchMessage(&sMessage);
            }
        }
      input_processor.GetActions(tp->input_held, tp->input_pressed, tp->input_released);
      game->Tick();
      game->DrawFrame();
      Sleep(20);
   }
   //game->TeardownDisplay();
   delete game;
   delete tp;
}
