#include "TouchData.h"
#include <cassert>
#include "../Log.h"


TouchData::TouchData()
{
   mTouchCount = 0;
   mMaxTouchCount = 0;
}

TouchData::TouchData(Vector2i position)
{
   mTouchCount = 1;
   mMaxTouchCount = 1;
   mTouchPosition[0] = position;
}

TouchData::TouchData(TouchData original, Vector2i position)
{
   if(original.mTouchCount < 2)
   {
      mTouchPosition[mTouchCount] = position;
      mTouchCount = original.mTouchCount + 1;
      mMaxTouchCount = std::max(mTouchCount, original.mMaxTouchCount);
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
