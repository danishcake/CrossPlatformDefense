#pragma once
#include "../vmath.h"

class CursorMoveMessage
{
public:
   Vector3f cursor_position;
   
   CursorMoveMessage(Vector3f position)
   {
      cursor_position = position;
   }
};
