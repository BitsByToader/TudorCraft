//
//  Chunk.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 18.11.2022.
//

#include "Chunk.hpp"

#include <simd/simd.h>
#include "AAPLUtilities.h"
#include "Math3D.hpp"
#include "World.hpp"

Chunk::Chunk(int x, int y, int z) {
    m_xCoordinate = x;
    m_yCoordinate = y;
    m_zCoordinate = z;
    
    m_blocks = new Block[BLOCKS_NUM_TOTAL];
    
    // Initialize the chunk with empty blocks
    for ( int i = 0; i < BLOCKS_NUM_TOTAL; i++ ) {
        m_blocks[i].state = nullptr;
        
        for ( int j = 0; j < 6; j++ ) {
            m_blocks[i].faceIndices[j] = -1;
        }
    }
};

Chunk::~Chunk() {
    delete[] m_blocks;
}

Block *Chunk::getBlockAt(int x, int y, int z) {
    if ( x >= 0 && y >= 0 && z >= 0 ) {
        return &(m_blocks[y * BLOCKS_NUM_X * BLOCKS_NUM_Z + z*BLOCKS_NUM_X + x]);
    } else {
        return nullptr;
    }
};

void Chunk::calculateMesh(Renderer *renderer) {
    using namespace Math3D;
    using namespace simd;
    
    const float scl = BLOCK_SIZE;
    
    int *instanceCount = renderer->instanceCount();
    InstanceData *buffer = renderer->instanceBuffer();

    const float4x4 scale = makeScale( (float3){ scl, scl, scl } );

    for ( int k = 0; k < BLOCKS_NUM_Y; k++ ) {
        for ( int j = 0; j < BLOCKS_NUM_Z; j++ ) {
            for ( int i = 0; i < BLOCKS_NUM_X; i++ ) {
                Block *currBlock = getBlockAt(i, j, k);
                
                // Only check blocks that aren't air
                if ( currBlock->state != nullptr ) {
                    m_blockCount++;
                    
                    // Create the main block position
                    float4x4 blockPositionTranslationMatrix =
                        makeTranslate((float3) {m_xCoordinate * BLOCKS_NUM_X + scl*i,
                                                m_yCoordinate * BLOCKS_NUM_Y + scl*k,
                                                m_zCoordinate * BLOCKS_NUM_Z - scl*j} );

                    if ( j == 0 || getBlockAt(i, k, j-1)->state == nullptr ) {
                        // Front of block
                        buffer[*instanceCount].transform = blockPositionTranslationMatrix * scale;
                        buffer[*instanceCount].normalTransform = Math3D::discardTranslation(buffer[*instanceCount].transform);
                        buffer[*instanceCount].textureId = currBlock->state->frontTexture();
                        currBlock->faceIndices[Faces::Front] = *instanceCount;
                        
                        (*instanceCount)++;
                    }

                    if ( i == 0 || getBlockAt(i-1, k, j)->state == nullptr ) {
                        // Left of block
                        buffer[*instanceCount].transform = blockPositionTranslationMatrix * scale * moveFaceToLeft();
                        buffer[*instanceCount].normalTransform = discardTranslation(buffer[*instanceCount].transform);
                        buffer[*instanceCount].textureId = currBlock->state->leftTexture();
                        currBlock->faceIndices[Faces::Left] = *instanceCount;
                        
                        (*instanceCount)++;
                    }

                    if ( j == BLOCKS_NUM_Z - 1 || getBlockAt(i, k, j+1)->state == nullptr ){
                        // Back of block
                        buffer[*instanceCount].transform = blockPositionTranslationMatrix * scale * moveFaceToBack();
                        buffer[*instanceCount].normalTransform = discardTranslation(buffer[*instanceCount].transform);
                        buffer[*instanceCount].textureId = currBlock->state->backTexture();
                        currBlock->faceIndices[Faces::Back] = *instanceCount;
                        
                        (*instanceCount)++;
                    }

                    if ( i == BLOCKS_NUM_X-1 || getBlockAt(i+1, k, j)->state == nullptr ){
                        // Right of block
                        buffer[*instanceCount].transform = blockPositionTranslationMatrix * scale * moveFaceToRight();
                        buffer[*instanceCount].normalTransform = discardTranslation(buffer[*instanceCount].transform);
                        buffer[*instanceCount].textureId = currBlock->state->rightTexture();
                        currBlock->faceIndices[Faces::Right] = *instanceCount;
                        
                        (*instanceCount)++;
                    }
                    
                    if ( k == BLOCKS_NUM_Y - 1 || getBlockAt(i, k+1, j)->state == nullptr ){
                        // Top of block
                        buffer[*instanceCount].transform = blockPositionTranslationMatrix * scale * moveFaceToTop();
                        buffer[*instanceCount].normalTransform = discardTranslation(buffer[*instanceCount].transform);
                        buffer[*instanceCount].textureId = currBlock->state->topTexture();
                        currBlock->faceIndices[Faces::Top] = *instanceCount;
                        
                        (*instanceCount)++;
                    }

                    if ( k == 0 || getBlockAt(i, k-1, j)->state == nullptr ) {
                        // Bottom of block
                        buffer[*instanceCount].transform = blockPositionTranslationMatrix * scale * moveFaceToBottom();
                        buffer[*instanceCount].normalTransform = discardTranslation(buffer[*instanceCount].transform);
                        buffer[*instanceCount].textureId = currBlock->state->bottomTexture();
                        currBlock->faceIndices[Faces::Bottom] = *instanceCount;
                        
                        (*instanceCount)++;
                    }
                }
            }
        }
    }
};

void Chunk::placeBlockAt(int x, int y, int z, BlockState *state, Renderer *renderer, World *world) {
    using namespace simd;
    using namespace Math3D;
    
    InstanceData *buffer = renderer->instanceBuffer();
    int *count = renderer->instanceCount();
    
    Block *currBlock = getBlockAt(x, y, z);
    Block *nextBlock;
    
    currBlock->state = state;
    
    const float scl = BLOCK_SIZE;
    const float4x4 scale = makeScale( (float3){ scl, scl, scl } );
    
    if ( state != nullptr ) {
        // We're placing a block.
        m_blockCount++;
        
        float4x4 blockPositionTranslationMatrix =
            makeTranslate((float3) {m_xCoordinate * BLOCKS_NUM_X + scl*x,
                                    m_yCoordinate * BLOCKS_NUM_Y + scl*y,
                                    m_zCoordinate * BLOCKS_NUM_Z - scl*z} );
        
        
        // Front of block
        nextBlock = world->getBlockAt(x, y, z-1);
        if ( nextBlock == nullptr || nextBlock->state == nullptr ) {
            // The block doesn't exist or it is an air block.
            // Draw face.
            buffer[*count].transform = blockPositionTranslationMatrix * scale;
            buffer[*count].normalTransform = Math3D::discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = currBlock->state->frontTexture();
            buffer[*count].blockCoordinates = {x, y, z};
            currBlock->faceIndices[Faces::Front] = *count;
            
            ++(*count);
            
        } else {
            // The block exists and it isn't an air block.
            // Don't draw face, but remove the neighbour's face.
            renderer->removeInstanceAt(nextBlock->faceIndices[Faces::Back]);
            nextBlock->faceIndices[Faces::Back] = -1;
        }
        
        // Left of block
        nextBlock = world->getBlockAt(x-1, y, z);
        if ( nextBlock == nullptr || nextBlock->state == nullptr ) {
            buffer[*count].transform = blockPositionTranslationMatrix * scale * moveFaceToLeft();
            buffer[*count].normalTransform = discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = currBlock->state->leftTexture();
            buffer[*count].blockCoordinates = {x, y, z};
            currBlock->faceIndices[Faces::Left] = *count;
            
            ++(*count);
        } else {
            renderer->removeInstanceAt(nextBlock->faceIndices[Faces::Right]);
            nextBlock->faceIndices[Faces::Right] = -1;
        }
       
        // Back of block
        nextBlock = world->getBlockAt(x, y, z+1);
        if ( nextBlock == nullptr || nextBlock->state == nullptr ) {
            buffer[*count].transform = blockPositionTranslationMatrix * scale * moveFaceToBack();
            buffer[*count].normalTransform = discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = currBlock->state->backTexture();
            buffer[*count].blockCoordinates = {x, y, z};
            currBlock->faceIndices[Faces::Back] = *count;
            
            ++(*count);
        } else {
            renderer->removeInstanceAt(nextBlock->faceIndices[Faces::Front]);
            nextBlock->faceIndices[Faces::Front] = -1;
        }
        
        // Right of block
        nextBlock = world->getBlockAt(x+1, y, z);
        if ( nextBlock == nullptr || nextBlock->state == nullptr ) {
//            printf("\tDrawing right face.\n");
            buffer[*count].transform = blockPositionTranslationMatrix * scale * moveFaceToRight();
            buffer[*count].normalTransform = discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = currBlock->state->rightTexture();
            buffer[*count].blockCoordinates = {x, y, z};
            currBlock->faceIndices[Faces::Right] = *count;
            
            ++(*count);
        } else {
            renderer->removeInstanceAt(nextBlock->faceIndices[Faces::Left]);
            nextBlock->faceIndices[Faces::Left] = -1;
        }
        
        // Top of block
        nextBlock = world->getBlockAt(x, y+1, z);
        if ( nextBlock == nullptr || nextBlock->state == nullptr ) {
//            printf("\tDrawing top face.\n");
            buffer[*count].transform = blockPositionTranslationMatrix * scale * moveFaceToTop();
            buffer[*count].normalTransform = discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = currBlock->state->topTexture();
            buffer[*count].blockCoordinates = {x, y, z};
            currBlock->faceIndices[Faces::Top] = *count;
            
            ++(*count);
        } else {
            renderer->removeInstanceAt(nextBlock->faceIndices[Faces::Bottom]);
            nextBlock->faceIndices[Faces::Bottom] = -1;
        }
        
        // Bottom of block
        nextBlock = world->getBlockAt(x, y-1, z);
        if ( nextBlock == nullptr || nextBlock->state == nullptr ) {
            buffer[*count].transform = blockPositionTranslationMatrix * scale * moveFaceToBottom();
            buffer[*count].normalTransform = discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = currBlock->state->bottomTexture();
            buffer[*count].blockCoordinates = {x, y, z};
            currBlock->faceIndices[Faces::Bottom] = *count;
            
            ++(*count);
        } else {
            renderer->removeInstanceAt(nextBlock->faceIndices[Faces::Top]);
            nextBlock->faceIndices[Faces::Top] = -1;
        }
    } else {
        // We're placing an air block, aka we're removing the block
        m_blockCount--;
        
        float4x4 blockPositionTranslationMatrix;
        
        // Front of block
        nextBlock = world->getBlockAt(x, y, z-1);
        if ( nextBlock != nullptr && nextBlock->state != nullptr ) {
            blockPositionTranslationMatrix=
                makeTranslate((float3) {m_xCoordinate * BLOCKS_NUM_X + scl*x,
                                        m_yCoordinate * BLOCKS_NUM_Y + scl*y,
                                        m_zCoordinate * BLOCKS_NUM_Z - scl*(z-1)} );
            
            buffer[*count].transform = blockPositionTranslationMatrix * scale * moveFaceToBack();
            buffer[*count].normalTransform = Math3D::discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = nextBlock->state->backTexture();
            buffer[*count].blockCoordinates = {x, y, z-1};
            nextBlock->faceIndices[Faces::Back] = *count;
            
            ++(*count);
        } else {
            renderer->removeInstanceAt(currBlock->faceIndices[Faces::Front]);
            currBlock->faceIndices[Faces::Front] = -1;
        }
        
        // Left of block
        nextBlock = world->getBlockAt(x-1, y, z);
        if ( nextBlock != nullptr && nextBlock->state != nullptr ) {
            blockPositionTranslationMatrix =
                makeTranslate((float3) {m_xCoordinate * BLOCKS_NUM_X + scl*(x-1),
                                        m_yCoordinate * BLOCKS_NUM_Y + scl*y,
                                        m_zCoordinate * BLOCKS_NUM_Z - scl*z} );
            
            buffer[*count].transform = blockPositionTranslationMatrix * scale * moveFaceToRight();
            buffer[*count].normalTransform = discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = nextBlock->state->rightTexture();
            buffer[*count].blockCoordinates = {x-1, y, z};
            nextBlock->faceIndices[Faces::Right] = *count;
            
            ++(*count);
        } else {
            renderer->removeInstanceAt(currBlock->faceIndices[Faces::Left]);
            currBlock->faceIndices[Faces::Left] = -1;
        }
       
        // Back of block
        nextBlock = world->getBlockAt(x, y, z+1);
        if ( nextBlock != nullptr && nextBlock->state != nullptr ) {
            blockPositionTranslationMatrix =
                makeTranslate((float3) {m_xCoordinate * BLOCKS_NUM_X + scl*x,
                                        m_yCoordinate * BLOCKS_NUM_Y + scl*y,
                                        m_zCoordinate * BLOCKS_NUM_Z - scl*(z+1)} );
            
            buffer[*count].transform = blockPositionTranslationMatrix * scale;
            buffer[*count].normalTransform = discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = nextBlock->state->frontTexture();
            buffer[*count].blockCoordinates = {x, y, z+1};
            nextBlock->faceIndices[Faces::Front] = *count;
            
            ++(*count);
        } else {
            renderer->removeInstanceAt(currBlock->faceIndices[Faces::Back]);
            currBlock->faceIndices[Faces::Back] = -1;
        }
        
        // Right of block
        nextBlock = world->getBlockAt(x+1, y, z);
        if ( nextBlock != nullptr && nextBlock->state != nullptr ) {
            blockPositionTranslationMatrix =
                makeTranslate((float3) {m_xCoordinate * BLOCKS_NUM_X + scl*(x+1),
                                        m_yCoordinate * BLOCKS_NUM_Y + scl*y,
                                        m_zCoordinate * BLOCKS_NUM_Z - scl*z} );
            
            buffer[*count].transform = blockPositionTranslationMatrix * scale * moveFaceToLeft();
            buffer[*count].normalTransform = discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = nextBlock->state->leftTexture();
            buffer[*count].blockCoordinates = {x+1, y, z};
            nextBlock->faceIndices[Faces::Left] = *count;
            
            ++(*count);
        } else {
            renderer->removeInstanceAt(currBlock->faceIndices[Faces::Right]);
            currBlock->faceIndices[Faces::Right] = -1;
        }
        
        // Top of block
        nextBlock = world->getBlockAt(x, y+1, z);
        if ( nextBlock != nullptr && nextBlock->state != nullptr ) {
            blockPositionTranslationMatrix =
                makeTranslate((float3) {m_xCoordinate * BLOCKS_NUM_X + scl*x,
                                        m_yCoordinate * BLOCKS_NUM_Y + scl*(y+1),
                                        m_zCoordinate * BLOCKS_NUM_Z - scl*z} );
            
            buffer[*count].transform = blockPositionTranslationMatrix * scale * moveFaceToBottom();
            buffer[*count].normalTransform = discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = nextBlock->state->bottomTexture();
            buffer[*count].blockCoordinates = {x, y+1, z};
            nextBlock->faceIndices[Faces::Bottom] = *count;
            
            ++(*count);
        } else {
            renderer->removeInstanceAt(currBlock->faceIndices[Faces::Top]);
            currBlock->faceIndices[Faces::Top] = -1;
        }
        
        // Bottom of block
        nextBlock = world->getBlockAt(x, y-1, z);
        if ( nextBlock != nullptr && nextBlock->state != nullptr ) {
            blockPositionTranslationMatrix =
                makeTranslate((float3) {m_xCoordinate * BLOCKS_NUM_X + scl*x,
                                        m_yCoordinate * BLOCKS_NUM_Y + scl*(y-1),
                                        m_zCoordinate * BLOCKS_NUM_Z - scl*z} );
            
            buffer[*count].transform = blockPositionTranslationMatrix * scale * moveFaceToTop();
            buffer[*count].normalTransform = discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = nextBlock->state->topTexture();
            buffer[*count].blockCoordinates = {x, y-1, z};
            nextBlock->faceIndices[Faces::Top] = *count;
            
            ++(*count);
        } else {
            renderer->removeInstanceAt(currBlock->faceIndices[Faces::Bottom]);
            currBlock->faceIndices[Faces::Bottom] = -1;
        }
    }
};

void Chunk::setBlockAt(int x, int y, int z, BlockState *state) { 
    this->getBlockAt(x, y, z)->state = state;
}

