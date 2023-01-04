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

bool Collidable::checkForCollisionsWithWorld(float x, float y, float z) {
    for ( float k = floor(y / 10); k <= floor((y+m_height)/10); k++ ) {
        for ( float j = floor(-z / 10); j <= floor(-(z+m_depth)/10); j++ ) {
            for ( float i = floor(x / 10); i <= floor((x+m_width)/10); i++ ) {
                if ( m_world->getBlockAt(i, k, j)->state != nullptr ) {
                    return true;
                }
            }
        }
    }
    
    return false;
};
