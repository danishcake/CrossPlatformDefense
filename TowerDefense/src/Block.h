#pragma once

/*
 * Types of blocks. Contains an enumeration, but intention is not to store directly. 
 * Use BlockType::BlockType instead for storage as it uses 1 byte
 */
namespace BlockType
{
    enum Enum
    {
        Empty     = 0, 
        Dirt      = 1,
        Grass     = 2,
        Stone     = 3,
        Concrete  = 4,
        Metal     = 5,
        Diamond   = 6
    };

    extern unsigned char blockHealth[];


    class BlockType
    {
    public:
        BlockType() :
          mValue(static_cast<unsigned char>(Empty))
        {
        }

        BlockType(Enum value) :
          mValue(static_cast<unsigned char>(value))
        {
        }
        
        Enum Value() const
        {
            return static_cast<Enum>(mValue);
        }
    private:
        unsigned char mValue;
    };
}
/*
 * Represents all the detail about a block apart from it's position which 
 * can be calculated from the storage position
 */
class Block
{
public:
    Block(BlockType::Enum bt)
    {
        blockType = bt;
        health = BlockType::blockHealth[blockType.Value()];
    }

    Block()
    {
       blockType = BlockType::Empty;
       health = BlockType::blockHealth[blockType.Value()];
    }

    BlockType::BlockType blockType;
    unsigned char health;
};
