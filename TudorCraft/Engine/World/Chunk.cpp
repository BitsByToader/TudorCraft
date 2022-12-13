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

#warning "All current problems stem from one fact alone: When you remove an instance from the buffer, you don't update the block whose instance was moved to the previously deleted index. Fix this???"

Chunk::Chunk(int x, int y, int z) {
    m_xCoordinate = x;
    m_yCoordinate = y;
    m_zCoordinate = z;
    
    m_blocks = new Block[BLOCKS_NUM_TOTAL];
    
    // Initialize the chunk with empty blocks
    for ( int i = 0; i < BLOCKS_NUM_TOTAL; i++ ) {
        m_blocks[i].state = nullptr;
        m_blocks[i].frontInstanceIndex = -1;
        m_blocks[i].rightInstanceIndex = -1;
        m_blocks[i].backInstanceIndex = -1;
        m_blocks[i].leftInstanceIndex = -1;
        m_blocks[i].bottomInstanceIndex = -1;
        m_blocks[i].topInstanceIndex = -1;
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
                        currBlock->frontInstanceIndex = *instanceCount;
                        
                        (*instanceCount)++;
                    }

                    if ( i == 0 || getBlockAt(i-1, k, j)->state == nullptr ) {
                        // Left of block
                        buffer[*instanceCount].transform = blockPositionTranslationMatrix * scale * moveFaceToLeft();
                        buffer[*instanceCount].normalTransform = discardTranslation(buffer[*instanceCount].transform);
                        buffer[*instanceCount].textureId = currBlock->state->leftTexture();
                        currBlock->leftInstanceIndex = *instanceCount;
                        
                        (*instanceCount)++;
                    }

                    if ( j == BLOCKS_NUM_Z - 1 || getBlockAt(i, k, j+1)->state == nullptr ){
                        // Back of block
                        buffer[*instanceCount].transform = blockPositionTranslationMatrix * scale * moveFaceToBack();
                        buffer[*instanceCount].normalTransform = discardTranslation(buffer[*instanceCount].transform);
                        buffer[*instanceCount].textureId = currBlock->state->backTexture();
                        currBlock->backInstanceIndex = *instanceCount;
                        
                        (*instanceCount)++;
                    }

                    if ( i == BLOCKS_NUM_X-1 || getBlockAt(i+1, k, j)->state == nullptr ){
                        // Right of block
                        buffer[*instanceCount].transform = blockPositionTranslationMatrix * scale * moveFaceToRight();
                        buffer[*instanceCount].normalTransform = discardTranslation(buffer[*instanceCount].transform);
                        buffer[*instanceCount].textureId = currBlock->state->rightTexture();
                        currBlock->rightInstanceIndex = *instanceCount;
                        
                        (*instanceCount)++;
                    }
                    
                    if ( k == BLOCKS_NUM_Y - 1 || getBlockAt(i, k+1, j)->state == nullptr ){
                        // Top of block
                        buffer[*instanceCount].transform = blockPositionTranslationMatrix * scale * moveFaceToTop();
                        buffer[*instanceCount].normalTransform = discardTranslation(buffer[*instanceCount].transform);
                        buffer[*instanceCount].textureId = currBlock->state->topTexture();
                        currBlock->topInstanceIndex = *instanceCount;
                        
                        (*instanceCount)++;
                    }

                    if ( k == 0 || getBlockAt(i, k-1, j)->state == nullptr ) {
                        // Bottom of block
                        buffer[*instanceCount].transform = blockPositionTranslationMatrix * scale * moveFaceToBottom();
                        buffer[*instanceCount].normalTransform = discardTranslation(buffer[*instanceCount].transform);
                        buffer[*instanceCount].textureId = currBlock->state->bottomTexture();
                        currBlock->bottomInstanceIndex = *instanceCount;
                        
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
        printf("Placing block at: %d %d %d\n", x, y, z);
        
        // We're placing a block.
        m_blockCount++;
        
        float4x4 blockPositionTranslationMatrix =
            makeTranslate((float3) {m_xCoordinate * BLOCKS_NUM_X + scl*x,
                                    m_yCoordinate * BLOCKS_NUM_Y + scl*y,
                                    m_zCoordinate * BLOCKS_NUM_Z - scl*z} );
        
        
        // Front of block
        nextBlock = world->getBlockAt(x, y, z-1);
        if ( nextBlock == nullptr || nextBlock->state == nullptr ) {
            printf("\tDrawing front face.\n");
            
            // The block doesn't exist or it is an air block.
            // Draw face.
            buffer[*count].transform = blockPositionTranslationMatrix * scale;
            buffer[*count].normalTransform = Math3D::discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = currBlock->state->frontTexture();
            currBlock->frontInstanceIndex = *count;
            
            ++(*count);
            
        } else {
            printf("\tRemoving back face.\n");
            // The block exists and it isn't an air block.
            // Don't draw face, but remove the neighbour's face.
            renderer->removeInstanceAt(nextBlock->backInstanceIndex);
            nextBlock->backInstanceIndex = -1;
        }
        
        // Left of block
        nextBlock = world->getBlockAt(x-1, y, z);
        if ( nextBlock == nullptr || nextBlock->state == nullptr ) {
            printf("\tDrawing left face.\n");
            buffer[*count].transform = blockPositionTranslationMatrix * scale * moveFaceToLeft();
            buffer[*count].normalTransform = discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = currBlock->state->leftTexture();
            currBlock->leftInstanceIndex = *count;
            
            ++(*count);
        } else {
            printf("\tRemoving right face.\n");
            renderer->removeInstanceAt(nextBlock->rightInstanceIndex);
            nextBlock->rightInstanceIndex = -1;
        }
       
        // Back of block
        nextBlock = world->getBlockAt(x, y, z+1);
        if ( nextBlock == nullptr || nextBlock->state == nullptr ) {
            printf("\tDrawing back face.\n");
            buffer[*count].transform = blockPositionTranslationMatrix * scale * moveFaceToBack();
            buffer[*count].normalTransform = discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = currBlock->state->backTexture();
            currBlock->backInstanceIndex = *count;
            
            ++(*count);
        } else {
            printf("\tRemoving front face.\n");
            renderer->removeInstanceAt(nextBlock->frontInstanceIndex);
            nextBlock->frontInstanceIndex = -1;
        }
        
        // Right of block
        nextBlock = world->getBlockAt(x+1, y, z);
        if ( nextBlock == nullptr || nextBlock->state == nullptr ) {
            printf("\tDrawing right face.\n");
            buffer[*count].transform = blockPositionTranslationMatrix * scale * moveFaceToRight();
            buffer[*count].normalTransform = discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = currBlock->state->rightTexture();
            currBlock->rightInstanceIndex = *count;
            
            ++(*count);
        } else {
            printf("\tRemoving left face.\n");
            renderer->removeInstanceAt(nextBlock->leftInstanceIndex);
            nextBlock->leftInstanceIndex = -1;
        }
        
        // Top of block
        nextBlock = world->getBlockAt(x, y+1, z);
        if ( nextBlock == nullptr || nextBlock->state == nullptr ) {
            printf("\tDrawing top face.\n");
            buffer[*count].transform = blockPositionTranslationMatrix * scale * moveFaceToTop();
            buffer[*count].normalTransform = discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = currBlock->state->topTexture();
            currBlock->topInstanceIndex = *count;
            
            ++(*count);
        } else {
            printf("\tRemoving bottom face.\n");
            renderer->removeInstanceAt(nextBlock->bottomInstanceIndex);
            nextBlock->bottomInstanceIndex = -1;
        }
        
        // Bottom of block
        nextBlock = world->getBlockAt(x, y-1, z);
        if ( nextBlock == nullptr || nextBlock->state == nullptr ) {
            printf("\tDrawing bottom face.\n");
            buffer[*count].transform = blockPositionTranslationMatrix * scale * moveFaceToBottom();
            buffer[*count].normalTransform = discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = currBlock->state->bottomTexture();
            currBlock->bottomInstanceIndex = *count;
            
            ++(*count);
        } else {
            printf("\tRemoving top face.\n");
            renderer->removeInstanceAt(nextBlock->topInstanceIndex);
            nextBlock->topInstanceIndex = -1;
        }
    } else {
        // We're placing an air block, aka we're removing the block
        m_blockCount--;
        
        float4x4 blockPositionTranslationMatrix =
            makeTranslate((float3) {m_xCoordinate * BLOCKS_NUM_X + scl*x,
                                    m_yCoordinate * BLOCKS_NUM_Y + scl*y,
                                    m_zCoordinate * BLOCKS_NUM_Z - scl*z} );
        
        // Front of block
        nextBlock = world->getBlockAt(x, y, z-1);
        if ( nextBlock != nullptr && nextBlock->state != nullptr ) {
            AAPL_PRINT("Add back to next");
            buffer[*count].transform = blockPositionTranslationMatrix * scale * moveFaceToBack();
            buffer[*count].normalTransform = Math3D::discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = nextBlock->state->backTexture();
            nextBlock->backInstanceIndex = *count;
            
            ++(*count);
        } else {
            AAPL_PRINT("Remove front from curr");
            renderer->removeInstanceAt(currBlock->frontInstanceIndex);
            currBlock->frontInstanceIndex = -1;
        }
        
        // Left of block
        nextBlock = world->getBlockAt(x-1, y, z);
        if ( nextBlock != nullptr && nextBlock->state != nullptr ) {
            AAPL_PRINT("Add right to next");
            buffer[*count].transform = blockPositionTranslationMatrix * scale * moveFaceToRight();
            buffer[*count].normalTransform = discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = nextBlock->state->rightTexture();
            nextBlock->rightInstanceIndex = *count;
            
            ++(*count);
        } else {
            AAPL_PRINT("Remove left from curr");
            renderer->removeInstanceAt(currBlock->leftInstanceIndex);
            currBlock->leftInstanceIndex = -1;
        }
       
        // Back of block
        nextBlock = world->getBlockAt(x, y, z+1);
        if ( nextBlock != nullptr && nextBlock->state != nullptr ) {
            AAPL_PRINT("Add front to next");
            buffer[*count].transform = blockPositionTranslationMatrix * scale;
            buffer[*count].normalTransform = discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = nextBlock->state->frontTexture();
            nextBlock->frontInstanceIndex = *count;
            
            ++(*count);
        } else {
            AAPL_PRINT("Remove back from curr");
            renderer->removeInstanceAt(currBlock->backInstanceIndex);
            currBlock->backInstanceIndex = -1;
        }
        
        // Right of block
        nextBlock = world->getBlockAt(x+1, y, z);
        if ( nextBlock != nullptr && nextBlock->state != nullptr ) {
            AAPL_PRINT("Add left to next");
            buffer[*count].transform = blockPositionTranslationMatrix * scale * moveFaceToLeft();
            buffer[*count].normalTransform = discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = nextBlock->state->leftTexture();
            nextBlock->leftInstanceIndex = *count;
            
            ++(*count);
        } else {
            AAPL_PRINT("Remove right from curr");
            renderer->removeInstanceAt(currBlock->rightInstanceIndex);
            currBlock->rightInstanceIndex = -1;
        }
        
        // Top of block
        nextBlock = world->getBlockAt(x, y+1, z);
        if ( nextBlock != nullptr && nextBlock->state != nullptr ) {
            AAPL_PRINT("Add bottom to next");
            buffer[*count].transform = blockPositionTranslationMatrix * scale * moveFaceToBottom();
            buffer[*count].normalTransform = discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = nextBlock->state->bottomTexture();
            nextBlock->bottomInstanceIndex = *count;
            
            ++(*count);
        } else {
            AAPL_PRINT("Remove top from curr");
            renderer->removeInstanceAt(currBlock->topInstanceIndex);
            currBlock->topInstanceIndex = -1;
        }
        
        // Bottom of block
        nextBlock = world->getBlockAt(x, y-1, z);
        if ( nextBlock != nullptr && nextBlock->state != nullptr ) {
            AAPL_PRINT("Add top to next");
            buffer[*count].transform = blockPositionTranslationMatrix * scale * moveFaceToTop();
            buffer[*count].normalTransform = discardTranslation(buffer[*count].transform);
            buffer[*count].textureId = nextBlock->state->topTexture();
            nextBlock->topInstanceIndex = *count;
            
            ++(*count);
        } else {
            AAPL_PRINT("Remove bottom from curr");
            renderer->removeInstanceAt(currBlock->bottomInstanceIndex);
            currBlock->bottomInstanceIndex = -1;
        }
    }
};

void Chunk::setBlockAt(int x, int y, int z, BlockState *state) { 
    this->getBlockAt(x, y, z)->state = state;
}

