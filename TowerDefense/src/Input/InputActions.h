#pragma once
#include <map>
#include "TouchData.h"

/*
 * InputAction. Defines a basic message type that can be passed around.
 * Stick a structure over it to interpret data
 */
class InputAction
{
public:
   enum Enum
   {
      CameraRotateLeft,
      CameraRotateRight,
      HoldMove,
      Click
   };

   InputAction::Enum action;

   struct
   {
      //No CameraRotateLeftData payload
      //No CameraRotateRightData payload

      struct
      {
         TouchData start;
         TouchData prev;
         TouchData current;
      } HoldMoveData;

      struct
      {
         TouchData end;
      } ClickData;

   } data;
};

typedef std::map<InputAction::Enum, InputAction> ActionMap;

