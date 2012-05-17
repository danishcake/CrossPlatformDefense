#include "WorldBlocks.h"
#include <cmath>
#include "Bitmap.h"
#include "TickParameters.h"
#include "vmath-collisions.h"
#include "Log.h"

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

/*
 * Checks if a ray hits the world
 */
bool WorldBlocks::CheckRayIntersection(Vector3f ray_origin, Vector3f ray_unit, Vector3f* intersection_point)
{
   //Now check if collision occurs on each block
   Vector3f intersect_point;
   Vector3f intersection_normal;
   Vector3f closest_intersect_point;
   Vector3f closest_intersect_normal;
   float intersect_distance;

   float min_intersect_distance = FLT_MAX;
   for (int x = 0; x < WORLD_WIDTH; x++)
   {
      for (int z = 0; z < WORLD_BREADTH; z++)
      {
         Column col = getColumn(x, z);

         for(int y = 0; y < WORLD_HEIGHT; y++)
         {
            Block block = col.Get(y);
            if(block.blockType.Value() == BlockType::Empty)
               continue;

            if(Collisions3f::RayIntersectsAABB(ray_origin, ray_unit, Vector3f(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)), Vector3f(1.0f, 1.0f, 1.0f), intersect_point, intersect_distance, intersection_normal))
            {
               if(min_intersect_distance > intersect_distance)
               {
                  closest_intersect_point = Vector3f(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
                  closest_intersect_normal = intersection_normal;
                  min_intersect_distance = intersect_distance;
               }
            }
         }
      }
   }


   if (intersection_point)
   {
      *intersection_point = intersect_point;
      Log::Debug(__FILE__, "(%f, %f, %f)", intersect_point.x, intersect_point.y, intersect_point.z);
   }
   return (min_intersect_distance < FLT_MAX);
}
