#pragma once

#include "Component.h"
#include "../vmath.h"

class Position : public Component
{
public:
   virtual void Tick(TickParameters& tp) {}
   virtual void Teardown(TickParameters& tp) {}

   //Getter and setter
   void SetPosition(Vector3f position) {mPosition = position;}
   Vector3f GetPosition() const {return mPosition;}

   void SetX(const float x) {mPosition.x = x;}
   void SetY(const float y) {mPosition.y = y;}
   void SetZ(const float z) {mPosition.z = z;}
   float GetX() const {return mPosition.x;}
   float GetY() const {return mPosition.y;}
   float GetZ() const {return mPosition.z;}

private:
   Vector3f mPosition;
};