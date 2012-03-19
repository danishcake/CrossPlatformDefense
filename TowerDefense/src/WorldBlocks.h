#pragma once

#include "Block.h"
#include "Column.h"
#include "CommonDefs.h"
#include <string>

struct TickParameters;

typedef Block BlockData[WORLD_WIDTH][WORLD_BREADTH][WORLD_HEIGHT];

/*
 * World blocks is the storage class 
 */
class WorldBlocks
{
    public:
        /*
         * Default constructor creates a world that is 10 units dirt, 1 unit grass
         * and air above
         */
        WorldBlocks();
        
        /*
         * Initialises with a png heightmap
         * The top will be grass, the bottom stone and everything in between
         * will be dirt
         */
        static WorldBlocks* LoadFromPNG(TickParameters& tp, std::string filename);
        
        /*
         * Gets the dimension of the WorldBlocks
         */
         const int getWidth() const {return mWidth;}
         const int getBreadth() const {return mBreadth;}
         const int getHeight() const {return mHeight;}

         Column getColumn(int x, int y);

    private:
        /*
         * Data storage for blocks. See Block.h
         */
        BlockData mBlocks;
        /*
         * By convention these shall be X, Y and Z coordinates respectively
         */
        int mWidth;
        int mBreadth;
        int mHeight;

        int mInvalidated[WORLD_WIDTH][WORLD_BREADTH];
};
