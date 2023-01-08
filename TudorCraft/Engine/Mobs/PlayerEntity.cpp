//
//  PlayerEntity.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 03.01.2023.
//

#include "PlayerEntity.hpp"
#include "AAPLUtilities.h"

PlayerEntity::PlayerEntity(float x, float y, float z): Collidable(), Entity(), PhysicsConformable() {
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
    
    Entity::m_cameraY = 10.f;
    
    // Set up the collision
    Collidable::setSize(5.f, 17.5f, 5.f);
    
    // Set up physics
    PhysicsConformable::setDrag(0.5f);
};

void PlayerEntity::setSpeed(float x, float y, float z) {
    PhysicsConformable::setVelocity(Forces::Movement, {x, y, z});
};

void PlayerEntity::updatePosition(double timeMs) {
    PhysicsConformable::setVelocity(Forces::Gravity, {0.f, -10.f, 0.f});
    
    simd::float3 deltaPosition = PhysicsConformable::calculateDeltaPosition(timeMs);
    float x = deltaPosition.x;
    float y = deltaPosition.y;
    float z = deltaPosition.z;
    
    simd::float3 currentPosition = Entity::position();
    
    std::array<bool, 6> collisions = checkForCollisionsWithWorld(currentPosition.x,
                                                                 currentPosition.y,
                                                                 currentPosition.z,
                                                                 x, y, z);
    
//    printf("Collisions: ");
//    if ( collisions[Faces::Front] ) printf("Front ");
//    if ( collisions[Faces::Back] ) printf("Back ");
//    if ( collisions[Faces::Left] ) printf("Left ");
//    if ( collisions[Faces::Right] ) printf("Right ");
//    if ( collisions[Faces::Bottom] ) printf("Bottom ");
//    if ( collisions[Faces::Top] ) printf("Top ");
//    printf("\n");
    
    if ( (collisions[Faces::Front] && z < 0) || (collisions[Faces::Back] && z > 0) )
        z = 0;

    if ( (collisions[Faces::Right] && x > 0) || (collisions[Faces::Left] && x < 0) )
        x = 0;

    if ( (collisions[Faces::Top] && y > 0) || (collisions[Faces::Bottom] && y < 0) )
        y = 0;
    
    onGround = collisions[Faces::Bottom];

    Entity::setPosition(currentPosition.x + x, currentPosition.y + y, currentPosition.z + z);
};
