#pragma once

#include "../Input/IInputProcessor.h"
#define WIN32_LEAN_AND_MEAN 1
#define NOMINMAX
#include <windows.h>

class WindowsInputProcessor : public IInputProcessor
{
public:
   WindowsInputProcessor();

   virtual void GetActions(ActionMap& held, ActionMap& pressed, ActionMap& released);
   bool Process(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
   bool mLeftDown;
   int mX;
   int mY;
};
