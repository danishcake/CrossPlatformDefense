#include "CursorEventReceiver.h"
#include "Position.h"
#include "../GameObject.h"
#include "../WorldBlocks.h"

CursorEventReceiver::CursorEventReceiver(CursorAction::Enum action, WorldBlocks* blocks) :
   mAction(action), mBlocks(blocks)
{
}

void CursorEventReceiver::Initialise(TickParameters& tp, GameObject* owner)
{
   mPosition = owner->GetComponent<Position>();
   mCursorMoveSubscriber.Initialise(&tp.msg);
}

void CursorEventReceiver::Tick(TickParameters& tp)
{
   std::vector<CursorMoveMessage> messages = mCursorMoveSubscriber.GetMessages();

   if(messages.size() > 0)
   {
      for(std::vector<CursorMoveMessage>::iterator it = messages.begin(); it != messages.end(); ++it)
      {
         switch(mAction)
         {
         case CursorAction::PositionCursor:
            mPosition->SetPosition(it->cursor_position);
            mPositionLTV = it->cursor_position;
            break;
         case CursorAction::DeleteTop:
            if(mPositionLTV == it->cursor_position &&
               it->cursor_position.y > 0)
            {
               Column col = mBlocks->getColumn(it->cursor_position.x, it->cursor_position.z);
               col.Set(col.GetHeight(), BlockType::Empty);
            }
            mPosition->SetPosition(it->cursor_position);
            mPositionLTV = it->cursor_position;

            break;
         }
         
      }
   }
}
