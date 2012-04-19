#pragma once

#include "Component.h"
#include "../Messaging/CursorMoveMessage.h"
#include "../vmath.h"
#include <boost/bind.hpp>
#include <boost/signals2.hpp>

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
   typedef boost::signals2::signal<void (CursorAction::Enum action, TickParameters& tp)> OnClick;
   typedef OnClick::slot_type OnClickSlotType;

   OnClick mOnClick;

   Position* mPosition;
   Subscriber<CursorMoveMessage> mCursorMoveSubscriber;
   CursorAction::Enum mAction;
   WorldBlocks* mBlocks;
   Vector3f mPositionLTV;

   bool mDeleteAllowed;
   bool mAddAllowed;
public:
   CursorEventReceiver(CursorAction::Enum action, WorldBlocks* blocks);

   virtual void Initialise(TickParameters& tp, GameObject* owner);
   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp) {}

   void SetDeleteMode(int x, int y, TickParameters& tp);
   void SetAddMode(int x, int y, TickParameters& tp);
   void SetPositionMode(int x, int y, TickParameters& tp);
   void SetDeleteAllowed(bool allowed);
   void SetAddAllowed(bool allowed);
   void SubscribeForAction(OnClickSlotType slot);
};