//
//  Entity.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 29.12.2022.
//

#ifndef Entity_hpp
#define Entity_hpp

#include <vector>
#include <simd/simd.h>
#include "Math3D.hpp"
#include "EntityComponent.hpp"

class Entity {
public:
    ~Entity();
    
    void setPosition(float x, float y, float z);
    void setRotation(float pitch, float yaw);
    void addComponent(EntityComponent *c);
    
    simd::float4x4 transformMatrix();
    std::vector<EntityComponent *> components();
    
    simd::float3 cameraPosition();
    
    simd::float3 position();
    simd::float3 rotation();
    
    //TODO: MAKE ME PRIVATE!
    // Relative POV location of the entity
    float m_cameraX = 0.f;
    float m_cameraY = 0.f;
    float m_cameraZ = 0.f;
    
    EntityComponent* cameraBoundComponent;
    
private:
    void calculateTransform();
    
    // Entity ID as sent by the server
    int m_id = 0;
    
    // Position
    float m_x = 0.f;
    float m_y = 0.f;
    float m_z = 0.f;
    
    // Rotation
    float m_yaw = 0.f;
    float m_pitch = 0.f;
    
    // Components
    std::vector<EntityComponent *> m_components;
    
    // Transform matrix
    simd::float4x4 m_transformMatrix = matrix_identity_float4x4;
};

#endif /* Entity_hpp */
