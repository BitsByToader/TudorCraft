//
//  Chunk.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 18.11.2022.
//

#ifndef Chunk_hpp
#define Chunk_hpp

#include "Block.hpp"

// Defines the number of blocks there are in a chunk
#define BLOCKS_NUM_X 16
#define BLOCKS_NUM_Y 16
#define BLOCKS_NUM_Z 16
#define BLOCKS_NUM_TOTAL 16*16*16

class Chunk {
public:
    Chunk();
    
private:
    Block *m_blocks[BLOCKS_NUM_TOTAL];
};

#endif /* Chunk_hpp */
