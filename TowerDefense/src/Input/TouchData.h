#pragma once
#include "../vmath.h"

/*
 * TouchData represents a one or more touch positions
 * It also tracks the maximum number of points tracked 
 */
class TouchData
{
private:
   int mTouchCount;
   int mMaxTouchCount;
   Vector2i mTouchPosition[2];

public:
   TouchData();
   TouchData(Vector2i position);
   TouchData(TouchData original, Vector2i position);

   int GetTouchCount() const;
   int GetMaxTouchCount() const;
   Vector2i GetTouchCentre() const;
   Vector2i GetTouch(int index) const;
};
