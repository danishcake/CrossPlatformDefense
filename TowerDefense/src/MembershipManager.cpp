#include "MembershipManager.h"
#include <algorithm>

void MembershipManager::Register(MembershipType::Enum group, GameObject* go)
{
    mMembers[group].push_back(go);
}

void MembershipManager::Unregister(MembershipType::Enum group, GameObject* go)
{
    mMembers[group].erase(std::remove(mMembers[group].begin(), mMembers[group].end(), go), mMembers[group].end());
}

std::vector<GameObject*> MembershipManager::GetMembers(MembershipType::Enum group)
{
   return mMembers[group];
}
