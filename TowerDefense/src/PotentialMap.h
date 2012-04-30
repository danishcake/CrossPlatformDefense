#pragma once 

#include "CommonDefs.h"

class WorldBlocks;

/*
 * PotentialMap allows quick lookup of best direction of travel
 * towards any of a set of targets on a regular grid. Simply follow the 
 * steepest descent in the field.
 */
class PotentialMap
{
public:
   PotentialMap(WorldBlocks* wb, int max_drop, int max_rise);

   // Calculate the best direction to travel to get to target
   bool Guide(int x, int y, int& dx, int& dy);

   // Find the nearest navigable spot (quite expensive? 
   // might be worth caching results on Evaluate)
   void NearestNavigable(int x, int y, int& dx, int& dy);

   void Evaluate();
private:
   int mPotential[WORLD_WIDTH][WORLD_BREADTH];
   WorldBlocks* mWorldBlocks;
   int mMaxDrop;
   int mMaxRise;
};