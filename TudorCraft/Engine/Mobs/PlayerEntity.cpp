//
//  PlayerEntity.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 03.01.2023.
//

#include "PlayerEntity.hpp"

PlayerEntity::PlayerEntity(float x, float y, float z): Collidable(), Entity() {
    // Set up the entity
    int indices[] = {2, 2, 2, 2, 2, 2};
    
    EntityComponent *bottom = new EntityComponent;
    bottom->setRelativePosition(0.f, 0.f, 0.f);
    bottom->setSize(5.f, 12.f, 2.5f);
    bottom->setRotation(0.f, 0.f, 0.f);
    bottom->setTextures(indices);
    
    EntityComponent *top = new EntityComponent;
    top->setRelativePosition(0.f, 12.f, 2.5f);
    top->setSize(5.f, 5.f, 5.f);
    top->setRotation(0.f, 0.f, 0.f);
    top->setTextures(indices);
    
    Entity::addComponent(bottom);
    Entity::addComponent(top);
    Entity::cameraBoundComponent = top;
    
    Entity::setRotation(0.f, 0.f);
    Entity::setPosition(x,y,z);
    
    m_cameraY = 10.f;
    
    // Set up the collision
    Collidable::setSize(5.f, 20.f, 5.f);
};

void PlayerEntity::setPosition(float x, float y, float z) {
    if ( !checkForCollisionsWithWorld(x, y, z) )
        Entity::setPosition(x, y, z);
};
