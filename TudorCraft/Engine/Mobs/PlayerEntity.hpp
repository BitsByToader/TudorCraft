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

class PlayerEntity: public Entity, public Collidable {
public:
    PlayerEntity(float x, float y, float z);
    
    void setPosition(float x, float y, float z);
};

#endif /* PlayerEntity_hpp */
