#pragma once

#include "Component.h"
#include "../Messaging/CursorMoveMessage.h"
#include "../vmath.h"

class Position;

/*
 * CursorEventReceiver waits for CursorPositioningMessages 
 */
class CursorEventReceiver : public Component
{
private:
   Position* mPosition;
   Subscriber<CursorMoveMessage> mCursorMoveSubscriber;
public:
   virtual void Initialise(TickParameters& tp, GameObject* owner);
   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp) {}
};