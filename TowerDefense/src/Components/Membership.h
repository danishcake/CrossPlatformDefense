#pragma once

#include "Component.h"
#include "../vmath.h"
#include "../MembershipTypes.h"
#include "../MembershipManager.h"

/*
 * Manages membership to a group
 */
template <MembershipType::Enum group> class Membership : public Component
{
public:
   virtual void Initialise(TickParameters& tp, GameObject* owner) {mOwner = owner; tp.memberships.Register(group, mOwner);}
   virtual void Tick(TickParameters& tp) {}
   virtual void Teardown(TickParameters& tp) {tp.memberships.Unregister(group, mOwner);}
private:
};
