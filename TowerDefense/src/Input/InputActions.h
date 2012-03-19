#pragma once
#include <map>

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
      Tap,
      HoldStart,
      HoldMove,
      HoldEnd,
      Click
   };

   InputAction::Enum action;

   union
   {
      //No CameraRotateLeftData payload
      //No CameraRotateRightData payload
      struct
      {
         int x;               //Position
         int y;
      } TapData;

      struct
      {
         int x;               //Start position
         int y;
      } HoldStartData;

      struct
      {
         int sx;              //Start position
         int sy;
         int x;               //Current position
         int y;
         int dx;              //Change since last report
         int dy;
         int max_displacement;
         int ms_held;         //Time held
      } HoldMoveData;

      struct
      {
         int sx;              //Start position
         int sy;
         int ex;              //End position
         int ey;
         int max_displacement;
         int ms_held;         //Time held
      } HoldEndData;

      struct
      {
         int sx;              //Start position
         int sy;
         int ex;              //End position
         int ey;
         int max_displacement;
         int ms_held;         //Time held
      } ClickData;

   } data;
};

typedef std::map<InputAction::Enum, InputAction> ActionMap;

