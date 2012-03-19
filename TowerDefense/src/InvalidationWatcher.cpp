#include "InvalidationWatcher.h"
#include "WorldBlocks.h"
#include "Column.h"

InvalidationWatcher::InvalidationWatcher()
{
   for(int x = 0; x < WORLD_WIDTH; x++)
   {
      for(int z = 0; z < WORLD_BREADTH; z++)
      {
         mInvalidationCounters[x][z] = 0;
      }
   }
}


bool InvalidationWatcher::CheckInvalidation(const Column& column)
{
   int invalidation_count = column.GetInvalidated();
   int x = column.GetX();
   int z = column.GetZ();
   if(invalidation_count > mInvalidationCounters[x][z])
   {
      mInvalidationCounters[x][z] = invalidation_count;
      return true;
   }
   return false;
}

bool InvalidationWatcher::CheckInvalidation(WorldBlocks& world)
{
   bool changed = false;
   for(int x = 0; x < WORLD_WIDTH; x++)
   {
      for(int z = 0; z < WORLD_BREADTH; z++)
      {
         int invalidation_count = world.getColumn(x, z).GetInvalidated();
         if(invalidation_count > mInvalidationCounters[x][z])
         {
            mInvalidationCounters[x][z] = invalidation_count;
            changed = true;
         }
      }
   }
   return changed;
}
