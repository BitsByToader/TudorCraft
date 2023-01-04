//
//  Collidable.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 03.01.2023.
//

#ifndef Collidable_hpp
#define Collidable_hpp

#include "World.hpp"

/// Abstract class used to make something collide with the world and other `Collidable` objects.
///
/// This should only be used when being inherited and never by itself, since it's pretty much useless.
/// This class uses an Axis Aligned Bounding Box(AABB) to calculate if the object intersects with the world or other `Collidables`.
class Collidable {
public:
    Collidable();
    
    void setSize(float width, float height, float depth);
    
    bool checkForCollisionsWithWorld(float x, float y, float z);
    
private:
    // AABB size
    float m_width = 0.f;
    float m_height = 0.f;
    float m_depth = 0.f;
    
    // World pointer used to detect collisions with world blocks
    World *m_world = nullptr;
};

#endif /* Collidable_hpp */
