#include "Column.h"

Column::Column()
   : mColumn(0),
     mInvalidated(0),
     mX(0),
     mZ(0)
{
}

Column::Column(Block* column, int* invalidated, int x, int z)
   : mColumn(column),
     mInvalidated(invalidated),
     mX(x),
     mZ(z)
{
}

/* Sets all blocks in the column to empty*/
void Column::ClearAll()
{
   for(int i = 0; i < WORLD_HEIGHT; i++)
   {
      if(mColumn[i].blockType.Value() != BlockType::Empty)
      {
         Invalidate();
         mColumn[i] = BlockType::Empty;
      }
   }
}

/* Sets all blocks from base to height inclusive */
void Column::FillTo(int height, BlockType::Enum type)
{
   for(int i = 0; i <= height && i < WORLD_HEIGHT; i++)
   {
      if(mColumn[i].blockType.Value() != type)
      {
         Invalidate();
         mColumn[i] = type;
      }
   }
}

/* Clears all blocks from top to height exclusive */
void Column::ClearTo(int height)
{
   for(int i = WORLD_HEIGHT - 1; i > height && i >= 0; i++)
   {
      if(mColumn[i].blockType.Value() != BlockType::Empty)
      {
         Invalidate();
         mColumn[i] = BlockType::Empty;
      }
   }
}

/* Sets a single block type */
void Column::Set(int y, BlockType::Enum type)
{
   if(y >= 0 && y < WORLD_HEIGHT)
   {
      if(mColumn[y].blockType.Value() != type)
      {
         Invalidate();
         mColumn[y] = type;
      }
   }
}

/* Collapses a column, removing all gaps */
void Column::Collapse()
{
   for(int i = 0; i < WORLD_HEIGHT - 1; i++)
   {
      if(mColumn[i].blockType.Value() == BlockType::Empty)
      {
         for(int j = i + 1; j < WORLD_HEIGHT; j++)
         {
            mColumn[j-1] = mColumn[j];
         }
         mColumn[WORLD_HEIGHT - 1] = BlockType::Empty;
         Invalidate();
      }
   }
}

/* Gets the invalidation flag */
int Column::GetInvalidated() const
{
   return *mInvalidated;
}

void Column::Invalidate()
{
   ++*mInvalidated;
}

Block& Column::Get(int y)
{
	return mColumn[y];
}

/* Obtains the height of the highest block */
int Column::GetHeight()
{
   for(int i = WORLD_HEIGHT-1; i >= 0; i--)
   {
      if(mColumn[i].blockType.Value() != BlockType::Empty)
         return i;
   }
   // Assume there is a bedrock
   return 0;
}