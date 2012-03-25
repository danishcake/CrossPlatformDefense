#include "CursorEventReceiver.h"
#include "Position.h"

Position* mPosition;
void CursorEventReceiver::Initialise(TickParameters& tp, GameObject* owner)
{
   mCursorMoveSubscriber.Initialise(&tp.msg);
}

void CursorEventReceiver::Tick(TickParameters& tp)
{
   std::vector<CursorMoveMessage> messages = mCursorMoveSubscriber.GetMessages();

   if(messages.size() > 0)
   {
      for(std::vector<CursorMoveMessage>::iterator it = messages.begin(); it != messages.end(); ++it)
      {
         mPosition->SetPosition(it->cursor_position);
      }
   }
}
