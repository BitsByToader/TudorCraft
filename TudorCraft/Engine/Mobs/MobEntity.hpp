//
//  PlayerEntity.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 03.01.2023.
//

#ifndef PlayerEntity_hpp
#define PlayerEntity_hpp

#include "Entity.hpp"
#include "Collidable.hpp"
#include "PhysicsConformable.hpp"

class MobEntity: public Entity, public Collidable, public PhysicsConformable {
public:
    MobEntity(float x, float y, float z);
    
    void updatePosition(double timeMs);
    void setSpeed(float x, float y, float z);
    
    bool onGround = false;
};

#endif /* PlayerEntity_hpp */
