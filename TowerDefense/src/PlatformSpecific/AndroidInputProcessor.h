#pragma once
#include "Input\IInputProcessor.h"
#include <android_native_app_glue.h>

class AndroidInputProcessor : public IInputProcessor
{
private:
   bool mScreenHeld;
   int mScreenStartX;
   int mScreenStartY;
public:
   AndroidInputProcessor();
   virtual void GetActions(ActionMap& held, ActionMap& pressed, ActionMap& released);
   bool Process(AInputEvent* event);
};
