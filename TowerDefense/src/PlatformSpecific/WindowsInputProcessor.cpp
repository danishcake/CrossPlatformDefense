#include "WindowsInputProcessor.h"
#include <windowsx.h>
#include "../Log.h"

WindowsInputProcessor::WindowsInputProcessor()
 : mLeftDown(false), mX(0), mY(0)
{
}

void WindowsInputProcessor::GetActions(ActionMap& held, ActionMap& pressed, ActionMap& released)
{
   //Poll keyboard
   if(GetAsyncKeyState(VK_LEFT))
   {
      InputAction action;
      action.action = InputAction::CameraRotateLeft;
      mActions[InputAction::CameraRotateLeft] = action;
   }
   if(GetAsyncKeyState(VK_RIGHT))
   {
      InputAction action;
      action.action = InputAction::CameraRotateRight;
      mActions[InputAction::CameraRotateRight] = action;
   }

   //Poll mouse
   //Poll joysticks


   // Mouse held calculations
   mCurrentTouch = TouchData();
   if(mLeftDown)
   {
      InputAction action;
      mCurrentTouch.AddTouch(Vector2i(mX, mY));
   }



   //Calculate deltas
   IInputProcessor::GetActions(held, pressed, released);
}

bool WindowsInputProcessor::Process(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   bool handled = false;
   switch(msg)
   {
      case WM_MOUSEMOVE:
      case WM_MOUSEHOVER:
      {
         RECT clientRect;
         GetClientRect(hWnd, &clientRect);

         mX = GET_X_LPARAM(lParam);
         mY = GET_Y_LPARAM(lParam);

         if(mX >= clientRect.left && mX < clientRect.right &&
            mY >= clientRect.top && mY < clientRect.bottom)
         {
            if(GetCapture() != hWnd)
            {
               SetCapture(hWnd);
            }
         } else
         {
            ReleaseCapture();
            mLeftDown = false;
         }

         handled = true;
       }
         break;
      case WM_LBUTTONDOWN:
         mLeftDown = true;
         handled = true;
         break;
      case WM_LBUTTONUP:
         mLeftDown = false;
         handled = true;
         break;
      default:
         //Do nothing. Not interested in this message
         break;
   }

   return handled;
}

