#include "TouchData.h"
#include <cassert>
#include "../Log.h"


TouchData::TouchData()
{
   mTouchCount = 0;
}

void TouchData::AddTouch(Vector2i position)
{
   if(mTouchCount < 2)
   {
      mTouchPosition[mTouchCount] = position;
      mTouchCount++;
   } else
   {
      Log::Debug(__FILE__, "Can only handle 2 touch points");
   }
}

int TouchData::GetTouchCount() const
{
   return mTouchCount;
}

int TouchData::GetMaxTouchCount() const
{
   return mMaxTouchCount;
}

void TouchData::SetMaxTouchCount(int count)
{
   mMaxTouchCount = count;
}

Vector2i TouchData::GetTouchCentre() const
{
   assert(mTouchCount > 0);
   Vector2i ret;
   for (int i = 0; i < mTouchCount; i++)
   {
      ret += mTouchPosition[i];
   }
   ret /= mTouchCount;
   return ret;
}

Vector2i TouchData::GetTouch(int index) const
{
   assert(mTouchCount > index); // If this fails you need to check your call index against GetTouchCount
   return mTouchPosition[index];
}

float TouchData::GetTouchDistance() const
{
   assert(mTouchCount == 2);
   return static_cast<float>((mTouchPosition[1] - mTouchPosition[0]).length());

}
