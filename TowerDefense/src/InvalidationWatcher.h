#pragma once
#include "CommonDefs.h"

class Column;
class WorldBlocks;

/*
 * Maintains a counter for each column and checks to see if it increases
 * between calls.
 */
class InvalidationWatcher
{
private:
   int mInvalidationCounters[WORLD_WIDTH][WORLD_BREADTH];
public:
   InvalidationWatcher();

   /* Returns true if the column has changed since the last call
    * Returns true on first call */
   bool CheckInvalidation(const Column& column);
   /* Returns true if any change has been made to the world since 
    * the last call. Returns true on first call */
   bool CheckInvalidation(WorldBlocks& world);

   /* Returns true if any column in a range has changed 
    * Unlike CheckInvalidation calls this does not clear the change flag
    * Call AcknowledgeChanges to clear.
    * Queried range is clamped, so can pass -1 as sx etc */
   bool CheckRange(WorldBlocks& world, int sx, int sz, int dx, int dz);

   /* Acknowledges invalidations that have occurred so that 
    * CheckRange will return false until another change occurs */
   void AcknowledgeChanges(WorldBlocks& world);
};
