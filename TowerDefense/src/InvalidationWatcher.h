#pragma once
#include "CommonDefs.h"

class Column;
class WorldBlocks;

/*
 * Maintains a counter for each column checks to see if it increases
 * between calls.
 */
class InvalidationWatcher
{
private:
   int mInvalidationCounters[WORLD_WIDTH][WORLD_BREADTH];
public:
   InvalidationWatcher();

   bool CheckInvalidation(const Column& column);
   bool CheckInvalidation(WorldBlocks& world);
};
