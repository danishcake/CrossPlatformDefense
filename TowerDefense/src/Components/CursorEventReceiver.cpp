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
   // Handle cursor positioning messages to move and action cursor
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
               it->cursor_position.y > 1)
            {
               Column col = mBlocks->getColumn(static_cast<int>(it->cursor_position.x), static_cast<int>(it->cursor_position.z));
               col.Set(col.GetHeight(), BlockType::Empty);
               if (it->cursor_position.y > 2)
               {
                  Column col = mBlocks->getColumn(static_cast<int>(it->cursor_position.x), static_cast<int>(it->cursor_position.z));
                  col.Set(col.GetHeight(), BlockType::Grass);
               }

               mPositionLTV = it->cursor_position + Vector3f(0, -1, 0);
               mPosition->SetPosition(mPositionLTV);
            } else
            {
               mPositionLTV = it->cursor_position;
               mPosition->SetPosition(mPositionLTV);
            }
            break;
         case CursorAction::AddTop:
            if(mPositionLTV == it->cursor_position)
            {
               Column col = mBlocks->getColumn(static_cast<int>(it->cursor_position.x), static_cast<int>(it->cursor_position.z));
               if (col.GetHeight() < WORLD_HEIGHT - 1)
               {
                  col.Set(col.GetHeight() + 1, BlockType::Grass);
               }

               if (it->cursor_position.y > 2)
               {
                  Column col = mBlocks->getColumn(static_cast<int>(it->cursor_position.x), static_cast<int>(it->cursor_position.z));
                  col.Set(col.GetHeight() - 1, BlockType::Dirt);
               }

               mPositionLTV = it->cursor_position + Vector3f(0, 1, 0);
               mPosition->SetPosition(mPositionLTV);
            } else
            {
               mPositionLTV = it->cursor_position;
               mPosition->SetPosition(mPositionLTV);
            }
            break;
         }
      }
   }
}

void CursorEventReceiver::SetDeleteMode(int x, int y, TickParameters& tp)
{
   mAction = CursorAction::DeleteTop;
}

void CursorEventReceiver::SetAddMode(int x, int y, TickParameters& tp)
{
   mAction = CursorAction::AddTop;
}

void CursorEventReceiver::SetPositionMode(int x, int y, TickParameters& tp)
{
   mAction = CursorAction::PositionCursor;
}
