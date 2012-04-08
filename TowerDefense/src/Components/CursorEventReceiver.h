#pragma once

#include "Component.h"
#include "../Messaging/CursorMoveMessage.h"
#include "../vmath.h"

class Position;
class WorldBlocks;

namespace CursorAction
{
   enum Enum
   {
      PositionCursor,
      DeleteTop,
      AddTop
   };
}

/*
 * CursorEventReceiver waits for CursorPositioningMessages 
 */
class CursorEventReceiver : public Component
{
private:
   Position* mPosition;
   Subscriber<CursorMoveMessage> mCursorMoveSubscriber;
   CursorAction::Enum mAction;
   WorldBlocks* mBlocks;
   Vector3f mPositionLTV;
public:
   CursorEventReceiver(CursorAction::Enum action, WorldBlocks* blocks);

   virtual void Initialise(TickParameters& tp, GameObject* owner);
   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp) {}

   void SetDeleteMode(int x, int y, TickParameters& tp);
   void SetAddMode(int x, int y, TickParameters& tp);
   void SetPositionMode(int x, int y, TickParameters& tp);
};