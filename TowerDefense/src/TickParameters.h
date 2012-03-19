#pragma once
#include <vector>
#include "Messaging/MessageHub.h"
#include "MembershipManager.h"
#include "PlatformSpecific/IFile.h"
#include "Input/InputActions.h"
#include "DrawList.h"

class GameObject;
class Camera;

/*
 * Parameters passed to tick methods
 */
struct TickParameters
{
private:
   std::vector<GameObject*> spawned_gos;

public:
   float timespan;
   MessageRoot msg;
   MembershipManager memberships;
   IFile* file;
   ActionMap input_held;
   ActionMap input_pressed;
   ActionMap input_released;
   DrawList draw_list;

   Camera* camera;

   TickParameters(IFile* file);

   ~TickParameters()
   {
      delete file;
   }

   //Adds a new GO to the simulation. Will be initialised
   void Spawn(GameObject* spawn)
   {
      spawned_gos.push_back(spawn);
   }

   //Returns new GameObjects created with Spawn
   std::vector<GameObject*>& GetSpawned()
   {
      return spawned_gos;
   }

   //Clears list for next pass
   void ClearSpawned()
   {
      spawned_gos.clear();
   }
};

