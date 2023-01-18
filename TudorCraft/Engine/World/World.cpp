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
};

World::~World() {
    // Delete all the chunks from memory
    for ( auto it: m_chunks) {
        delete it.second;
    };
    m_chunks.clear();
    
    //TODO: Also reset the instance data buffer to blank out the renderer?
}

World *World::m_sharedObject = nullptr;
World *World::shared() {
    if ( m_sharedObject == nullptr ) {
        m_sharedObject = new World();
    }
    
    return m_sharedObject;
}

void World::destroySharedObject() {
    delete m_sharedObject;
    m_sharedObject = nullptr;
}

#warning Find a faster way to calculate these correctly
#define DIVF(x, y) ( floor((float)x / float(y)) )
#define REM(x, y) ( x - DIVF(x,y)*y )

Chunk* World::getChunkAt(int x, int y, int z) {
    Tuple3D chunkCoords = { DIVF(x, 16), DIVF(y, 16), DIVF(z, 16) };
    return m_chunks[chunkCoords];
};

Block *World::getBlockAt(int x, int y, int z) {
    Chunk *c = getChunkAt(x, y, z);
    
    if ( c != nullptr ) {
        return c->getBlockAt(REM(x, 16), REM(y, 16), REM(z, 16));
    } else {
        return nullptr;
    }
}

void World::placeBlockAt(int x, int y, int z, BlockState *state) {
    Chunk *c = getChunkAt(x, y, z);
    if ( c != nullptr ) {
        c->placeBlockAt(REM(x, 16), REM(y, 16), REM(z, 16), state);
    }
}

void World::loadChunk(Chunk *c) {
    m_chunks[{c->x(), c->y(), c->z()}] = c;
}

void World::highlightBlock(int x, int y, int z) {
    Block *b = getBlockAt(x, y, z);
    
    removeHightlight();
    
    InstanceData *faces = Renderer::shared()->instanceBuffer();
    for ( int i = 0; i < 6; i++ ) {
        if ( b->faceIndices[i] != -1 ) {
            faces[b->faceIndices[i]].highlighted = true;
        }
    }
    
    
    
    m_highlightedBlock = b;
};

void World::removeHightlight() {
    InstanceData *faces = Renderer::shared()->instanceBuffer();
    if ( m_highlightedBlock != nullptr ) {
        for ( int i = 0; i < 6; i++ ) {
            if ( m_highlightedBlock->faceIndices[i] != -1 ) {
                faces[m_highlightedBlock->faceIndices[i]].highlighted = false;
            }
        }
    }
    
    m_highlightedBlock = nullptr;
};





