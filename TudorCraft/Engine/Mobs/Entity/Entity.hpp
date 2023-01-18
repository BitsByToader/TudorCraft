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

/// Abstract class used to create mobs and players in game.
///
/// This should only be used when being inherited and never by itself, since it's pretty much useless.
/// It doesn't do collision checking, it doesn't apply any physics, etc. It only manages the components that make up
/// an entity visually and any camera binding logic.
class Entity {
public:
    ~Entity();
    
    /// Sets the position of the entity in the world. The method expects engine coordinates, not world coordinates.
    /// - Parameters:
    ///   - x: X engine coordinate.
    ///   - y: Y engine coordinate.
    ///   - z: Z engine coordinate.
    void setPosition(float x, float y, float z);
    
    /// Sets the rotation of the entity.
    /// - Parameters:
    ///   - pitch: Pitch angle of the entity.
    ///   - yaw: Yaw angle of the entity.
    void setRotation(float pitch, float yaw);
    
    /// Adds a component to the entity.
    ///
    /// **NOTE: ** Once a component has been added to an  entity, it is considered that the entity
    /// is now the parent of the component and it fully owns it, making it responsible for deallocating it.
    ///
    /// - Parameter c: The component to add.
    void addComponent(EntityComponent *c);
    
    /// Returns the transform matrix of the entity.
    simd::float4x4 transformMatrix();
    
    /// Returns a std vector of the entity's components.
    std::vector<EntityComponent *> components();
    
    /// Returns the relative position of the entity's camera to the entity's position.
    simd::float3 cameraPosition();
    
    /// Returns the position of the entity.
    simd::float3 position();
    
    /// Returns the rotation of the entity.
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
