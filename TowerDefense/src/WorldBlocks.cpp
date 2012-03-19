#include "WorldBlocks.h"
#include <cmath>
#include "Bitmap.h"
#include "TickParameters.h"

/*
 * Default constructor creates a world that is 10 units dirt, 1 unit grass
 * and air above
 */
WorldBlocks::WorldBlocks()
 : mWidth(WORLD_WIDTH),  mBreadth(WORLD_BREADTH), mHeight(WORLD_HEIGHT)
 
{
   for(int x = 0; x < mWidth; x++)
   {
      for(int y = 0; y < mBreadth; y++)
      {
         for(int z = 0; z < 10; z++)
         {
            mBlocks[x][y][z] = BlockType::Dirt;
         }
         mBlocks[x][y][10] = BlockType::Grass;

         mInvalidated[x][y] = 1;
      }
   }
}

WorldBlocks* WorldBlocks::LoadFromPNG(TickParameters& tp, std::string filename)
{
   WorldBlocks* world = new WorldBlocks();
   Bitmap bmp(Bitmap::LoadPNG(tp, filename, false));
   if(bmp.GetLoadState() == BitmapLoadState::OK && 
      bmp.GetWidth() == WORLD_WIDTH && bmp.GetHeight() == WORLD_BREADTH)
   {
      for(int x = 0; x < WORLD_WIDTH; x++)
      {
         for(int y = 0; y < WORLD_BREADTH; y++)
         {
            Column col = world->getColumn(x, y);
            col.ClearAll();
            int height = (bmp.GetPixel(x, y).r + 1) / 16;

            for(int z = 0; z < height; z++)
            {
               if(z == 0)
                  col.Set(z, BlockType::Stone);
               else if(z == height - 1)
                  col.Set(z, BlockType::Grass);
               else
                  col.Set(z, BlockType::Dirt);
            }
         }
      }
   }

   return world;
}

/*
 * Gets a column wrapping x/z
 */
Column WorldBlocks::getColumn(int x, int z)
{
   return Column(mBlocks[x][z], &mInvalidated[x][z], x, z);
}
