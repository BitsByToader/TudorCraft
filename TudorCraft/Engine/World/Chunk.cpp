//
//  Chunk.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 18.11.2022.
//

#include "Chunk.hpp"

Chunk::Chunk() {
    // Initialize all of the blocks in a chunk with air blocks.
    for ( int i = 0; i < BLOCKS_NUM_TOTAL; i++ ) {
        m_blocks[i] = nullptr;
    };
};
