//
//  Collidable.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 03.01.2023.
//

#include "Collidable.hpp"

#include "AAPLUtilities.h"
#include "Chunk.hpp"
#include <math.h>

Collidable::Collidable() {
    m_world = World::shared();
};

void Collidable::setSize(float width, float height, float depth) {
    m_width = width;
    m_height = height;
    m_depth = depth;
};

std::array<bool, 6> Collidable::checkForCollisionsWithWorld(float initialX, float initialY, float initialZ,
                                                float dirX, float dirY, float dirZ) {
    float x = initialX + dirX;
    float y = initialY + dirY;
    float z = initialZ + dirZ;
    
    std::array<bool, 6> collisions = {false, false, false, false, false, false};
    
    /*
     x ~~ i
     y ~~ j
     z ~~ k
     
     The code below checks all the faces of the AABB to see if there is any non-air block.
     TODO: Make the air block a normal Block state with collidable set to false and check if that value is false or not instead of just checking is state is nullptr.
     */
    
    Block *b = nullptr;
    
    for ( int i = floor(x/10); i <= floor((x+m_width)/10); i++ ) {
        for ( int j = floor(y/10); j <= floor((y+m_height)/10); j++ ) {
            // Substract/Add 0.1 to check for neighbour blocks in the world that are a bit further
            // a way than the actual AABB to prevent from returning multiple collisions .
            if ( dirZ > 0 ) {
                b = m_world->getBlockAt(i, j, floor(-z/10-0.1));
                if ( b == nullptr || (b != nullptr && b->state != nullptr) )
                    // Mark a collision if the block isn't an air block, or if there is no block at all
                    collisions[Faces::Back] = true;
            }
                
            
            
            if ( dirZ < 0 ) {
                b = m_world->getBlockAt(i, j, floor((-z+m_depth)/10+0.1));
                if ( b == nullptr || (b != nullptr && b->state != nullptr) )
                    collisions[Faces::Front] = true;
            }
        }
    }
    
    for ( int j = floor(y/10); j <= floor((y+m_height)/10); j++ ) {
        for ( int k = floor(-z/10); k <= floor((-z+m_depth)/10); k++ ) {
            if ( dirX < 0 ){
                b = m_world->getBlockAt(floor(x/10-0.1), j, k);
                if ( b == nullptr || (b != nullptr && b->state != nullptr) )
                    collisions[Faces::Left] = true;
            }
            
            if ( dirX > 0 ) {
                b = m_world->getBlockAt(floor((x+m_width)/10+0.1), j, k);
                if ( b == nullptr || (b != nullptr && b->state != nullptr) )
                    collisions[Faces::Right] = true;
            }
        }
    }
    
    for ( int i = floor(x/10); i <= floor((x+m_width)/10); i++ ) {
        for ( int k = floor(-z/10); k <= floor((-z+m_depth)/10); k++ ) {
            if ( dirY < 0 ) {
                b = m_world->getBlockAt(i, floor(y/10-0.2), k);
                if ( b == nullptr || (b != nullptr && b->state != nullptr) )
                    collisions[Faces::Bottom] = true;
            }
            
            
            if ( dirY > 0 ) {
                b = m_world->getBlockAt(i, floor((y+m_height)/10+0.2), k);
                if ( b == nullptr || (b != nullptr && b->state != nullptr) )
                    collisions[Faces::Top] = true;
            }
        }
    }
 
    return collisions;
};
