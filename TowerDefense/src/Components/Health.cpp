#include "Health.h"
#include "../GameObject.h"

void Health::Heal(int dmg)
{
   mHealth += dmg;
   if(mHealth > mHealthMax)
      mHealth = mHealthMax;
}

void Health::Damage(int dmg)
{
   mHealth -= dmg;
}

void Health::Tick(TickParameters &tp)
{
   if(mHealth <= 0)
      mOwner->Kill();
}