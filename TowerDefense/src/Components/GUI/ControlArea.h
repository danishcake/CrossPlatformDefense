#pragma once
#include "../Component.h"
#include "UDim.h"

class ControlArea;


/*
 * ControlArea represents a region that a GUI control occupies
 * It checks for click events witin it's region
 */
class ControlArea : public Component
{
protected:
   UDim mPosition;
   UDim mSize;

public:
   ControlArea();
   ControlArea(UDim position, UDim size);

   virtual void Tick(TickParameters& tp);
   virtual void Teardown(TickParameters& tp){};

   UDim& GetPosition() {return mPosition;}
   UDim& GetSize() {return mSize;}
};
