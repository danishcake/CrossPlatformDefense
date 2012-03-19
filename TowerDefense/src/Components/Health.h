#pragma once

#include "Component.h"
#include "../vmath.h"

/*
 * Health tracks the damage a game object has taken and marks it
 * removable when fully depleted
 */
class Health : public Component
{
public:
   Health(int health);

   void Heal(int dmg);
   void Damage(int dmg);

   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp) {}

private:
   int mHealth;
   int mHealthMax;
};