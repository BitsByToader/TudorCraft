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
    
    //TODO: Also reset the instance data buffer to blank out the renderer?
}

World *World::m_globalObject = nullptr;
World *World::shared() {
    if ( m_globalObject == nullptr ) {
        m_globalObject = new World();
    }
    
    return m_globalObject;
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
    
//    Renderer::shared()->m_gpuMutex.lock();
//    c->calculateMesh();
//    Renderer::shared()->m_gpuMutex.unlock();;
}

void World::calculateMeshes() {
    Renderer::shared()->m_gpuMutex.lock();
    for ( auto it: m_chunks ) {
        if ( it.second != nullptr ) {
            it.second->calculateMesh();
        }
    }
    Renderer::shared()->m_gpuMutex.unlock();
}





