//
//  World.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 18.11.2022.
//

#include "World.hpp"

#include "AAPLUtilities.h"
#include "Chunk.hpp"

World::World() {
    Chunk *chunk = new Chunk(0, 0, 0);
    
    for ( int i = 0; i < 16; i++ ) {
        for ( int j = 0; j < 16; j++ ) {
            for ( int k = 0; k < 16; k++ ) {
                chunk->setBlockAt(i, j, k, BlockState::GrassBlock());
            }
        }
    }
    
    chunk->calculateMesh();
    
    m_chunks[{0,0,0}] = chunk;
};

World::~World() {
    // Delete all the chunks from memory
    for ( auto it: m_chunks) {
        delete it.second;
    };
    
    //TODO: Also reset the instance data buffer to blank out the renderer?
}

World *World::m_globalObject = nullptr;
World *World::shared() {
    if ( m_globalObject == nullptr ) {
        m_globalObject = new World();
    }
    
    return m_globalObject;
}

#warning Use the actual chunk sizes instead of 16
#warning Exception handling!
Chunk* World::getChunkAt(int x, int y, int z) {
    Tuple3D chunkCoords = { x/16, y/16, z/16 };
    return m_chunks[chunkCoords];
};

Block *World::getBlockAt(int x, int y, int z) {
    Chunk *c = getChunkAt(x, y, z);
    if ( c != nullptr ) {
        std::cout << "Got block " << x%16<<","<<y%16<<","<<z%16 << " from chunk " << x/16<<","<<y/16<<","<< z/16 << std::endl;
        return c->getBlockAt(x%16, y%16, z%16);
    } else {
        return nullptr;
    }
}

void World::placeBlockAt(int x, int y, int z, BlockState *state) {
    Chunk *c = getChunkAt(x, y, z);
    if ( c != nullptr ) {
        c->placeBlockAt(x%16, y%16, z%16, state);
    }
}




