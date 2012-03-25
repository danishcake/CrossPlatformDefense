#pragma once

#include "../Component.h"
#include "../../Messaging/GuiTransitionControlMessage.h"

class ControlArea;
class ControlOutline;
class ControlText;

/*
 * ControlPopup contains a number of sub menus
 * and draws a region around them when expanded
 * ------------------------------------
 * |                                  |
 * |                                  |
 * |                                  |
 * |                                  |
 * |      -----------------------------
 * |-----|
 * |     |
 * |_____|
 */
class ControlPopup : public Component
{
private:
   std::vector<GameObject*> mSubControls;
   bool mExpanded;

public:
   ControlPopup();
   ~ControlPopup();

   virtual void Initialise(TickParameters& tp, GameObject* owner);
   virtual void InitialiseGraphics(TickParameters& tp);
   virtual void Tick(TickParameters& tp);
   virtual void Draw(TickParameters& tp);
   virtual void Teardown(TickParameters& tp);
   virtual void TeardownGraphics(TickParameters& tp);
};
