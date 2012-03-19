#pragma once

namespace GameStates
{
   enum Enum
   {
      Menu,
      Credits,
      Build,
      Defend
   };
}

class StateChangeMessage
{
public:
   GameStates::Enum game_state;
   
   StateChangeMessage(GameStates::Enum state)
   {
      game_state = state;
   }
};
