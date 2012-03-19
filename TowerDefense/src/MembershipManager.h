#pragma once
#include <vector>
#include "MembershipTypes.h"

class GameObject;

class MembershipManager
{
public:
   void Register(MembershipType::Enum group, GameObject* go);
   void Unregister(MembershipType::Enum group, GameObject* go);
   std::vector<GameObject*> GetMembers(MembershipType::Enum group);
private:
   std::vector<GameObject*> mMembers[MembershipType::N_MEMBERSHIP_TYPES];
};
