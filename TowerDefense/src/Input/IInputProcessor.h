#pragma once
#include "InputActions.h"

class IInputProcessor
{
protected:
   ActionMap mActions;
   ActionMap mActionsLastTime;
public:
   virtual void GetActions(ActionMap& held, ActionMap& pressed, ActionMap& released);
   void ClearActions();
};
