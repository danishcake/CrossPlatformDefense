#pragma once

#include "Block.h"
#include "CommonDefs.h"

/*
 * Column wraps a reference to a column of blocks. It provides
 * methods for manipulating the Column, and marks it as changed when they are called.
 */
class Column
{
public:
   Column();

   Column(Block* column, int* invalidated, int x, int z);

   /* Sets all blocks in the column to empty*/
   void ClearAll();

   /* Sets all blocks from base to height inclusive */
   void FillTo(int height, BlockType::Enum type);

   /* Clears all blocks from top to height exclusive */
   void ClearTo(int height);

   /* Sets a single block type */
   void Set(int y, BlockType::Enum type);

   /* Compresses the Column so no gaps remains */
   void Collapse();

   /* Gets the invalidation counter */
   int GetInvalidated() const;

   /* Obtains the current block type */
   Block& Get(int y);

   /* Obtains the height of the highest block */
   int GetHeight();

   /* Increments the invalidation counter */
   void Invalidate();

   int GetX() const { return mX; }
   int GetZ() const { return mZ; }

private:
   Block* mColumn;
   int* mInvalidated;
   int mX;
   int mZ;
};
