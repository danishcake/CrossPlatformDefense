#include "PotentialMap.h"
#include "CommonDefs.h"
#include "WorldBlocks.h"
#include <vector>
#include <limits>

PotentialMap::PotentialMap(WorldBlocks* wb, int max_drop, int max_rise) :
    mWorldBlocks(wb),
    mMaxDrop(max_drop),
    mMaxRise(max_rise)
{
    Evaluate();
}

void PotentialMap::Evaluate()
{
   //Clear all potentials
   for(int i = 0; i < WORLD_WIDTH; i++)
   {
      for(int j = 0; j < WORLD_BREADTH; j++)
      {
         mPotential[i][j] = -1;
      }
   }

   std::vector<int> mCells; //Cells marked in the last pass
   std::vector<int> mMarked; //Cells marked in this pass
   int reached_cells = 0;
   //TODO set initial mCells to target cells and mark as 0
   mCells.push_back((WORLD_BREADTH / 2) * WORLD_WIDTH + WORLD_WIDTH / 2);


   bool cell_reached = true;
   while(cell_reached && reached_cells < WORLD_WIDTH * WORLD_BREADTH)
   {
      cell_reached = false;
      for(std::vector<int>::iterator cell_it = mCells.begin(); cell_it != mCells.end(); ++cell_it)
      {
         int x = *cell_it % WORLD_WIDTH;
         int y = *cell_it / WORLD_WIDTH;

         int neighbours[4];
         neighbours[0] = x <= 0 ? -1                                         : (y + 0) * WORLD_WIDTH + (x - 1);
         neighbours[1] = y <= 0 ? -1                                         : (y - 1) * WORLD_WIDTH + (x - 0);
         neighbours[2] = x >= WORLD_WIDTH - 1 ? -1                           : (y + 0) * WORLD_WIDTH + (x + 1);
         neighbours[3] = y >= WORLD_BREADTH - 1 ? -1                         : (y + 1) * WORLD_WIDTH + (x - 0);

         Column col = mWorldBlocks->getColumn(x, y);
         for(int i = 0; i < 4; i++)
         {
            if(neighbours[i] != -1)
            {
               int xn = neighbours[i] % WORLD_WIDTH;
               int yn = neighbours[i] / WORLD_WIDTH;
               //Calculate change in height
               Column coln = mWorldBlocks->getColumn(xn, yn);
               int dh = col.GetHeight() - coln.GetHeight(); //+ve means I'm higher

               int potential = mPotential[x][y] + 100;
               if(((dh == 0) || (dh > 0 && dh <= mMaxRise) || (dh < 0 && dh >= -mMaxDrop)) &&
                  (mPotential[xn][yn] == -1 || mPotential[xn][yn] > potential))
               {
                  if(mPotential[xn][yn] == -1)
                     reached_cells++;
                  mPotential[xn][yn] = potential;
                  cell_reached = true;
                  mMarked.push_back(neighbours[i]);
               }
            }
         }
      }
      mCells = mMarked;
      mMarked.clear();
   }
}

void PotentialMap::Guide(int x, int y, int& dx, int& dy)
{
   int neighbours[4];
   neighbours[0] = x <= 0 ? -1                                        : (y + 0) * WORLD_WIDTH + (x - 1);
   neighbours[1] = y <= 0 ? -1                                        : (y - 1) * WORLD_WIDTH + (x - 0);
   neighbours[2] = x >= WORLD_WIDTH - 1 ? -1                          : (y + 0) * WORLD_WIDTH + (x + 1);
   neighbours[3] = y >= WORLD_BREADTH - 1 ? -1                        : (y + 1) * WORLD_WIDTH + (x - 0);


   // Find lowest of the neighbouring cells
   Column col = mWorldBlocks->getColumn(x, y);
   int lowest = std::numeric_limits<int>::max();
   for(int i = 0; i < 4; i++)
   {
      if(neighbours[i] != -1)
      {
         int xn = neighbours[i] % WORLD_WIDTH;
         int yn = neighbours[i] / WORLD_WIDTH;
         Column coln = mWorldBlocks->getColumn(xn, yn);
         int dh = col.GetHeight() - coln.GetHeight(); //+ve means I'm higher

         if(mPotential[xn][yn] < lowest && 
            mPotential[xn][yn] != -1 &&
            ((dh == 0) || (dh > 0 && dh <= mMaxRise) || (dh < 0 && dh >= -mMaxDrop)))
         {
            lowest = mPotential[xn][yn];
            dx = xn - x;
            dy = yn - y;
         }
      }
   }
}
