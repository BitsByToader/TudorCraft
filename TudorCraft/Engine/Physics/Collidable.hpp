//
//  Collidable.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 03.01.2023.
//

#ifndef Collidable_hpp
#define Collidable_hpp

#include <array>
#include "World.hpp"
#include "Faces.hpp"

/// Abstract class used to make something collide with the world and other `Collidable` objects.
///
/// This should only be used when being inherited and never by itself, since it's pretty much useless.
/// This class uses an Axis Aligned Bounding Box (AABB) to calculate if the object intersects with the world or other `Collidables`.
///
/// Note: Collisons with other `Collidables` haven't been implemented yet.
class Collidable {
public:
    /// Default constructer that initializes the `m_world` member to the shared value.
    Collidable();
    
    /// Sets the size of the AABB.
    /// - Parameters:
    ///   - width: Width in render units.
    ///   - height: Height in render units.
    ///   - depth: Depth in render units.
    void setSize(float width, float height, float depth);
    
    /// Checks if the AABB collides with any block in the world.
    ///
    /// The method will check for collisions only in the direction the AABB is moving in.
    /// As such, only the collisions that would prevent the AABB from moving in said direction will be returned.
    ///
    /// - Parameters:
    ///   - initialX: Initial X of the AABB in render coordinates.
    ///   - initialY: Initial Y of the AABB in render coordinates.
    ///   - initialZ: Initial Z of the AABB in render coordinates.
    ///   - dirX: The X member of the 3D direction vector the AABB is moving with.
    ///   - dirY: The Y member of the 3D direction vector the AABB is moving with.
    ///   - dirZ: The Z member of the 3D direction vector the AABB is moving with.
    ///
    /// - Returns: A six dimensional array which holds which faces were collided with. The array is mapped using the `Faces` enum
    std::array<bool, 6> checkForCollisionsWithWorld(float initialX, float initialY, float initialZ,
                                                    float dirX, float dirY, float dirZ);
    
private:
    // AABB size
    float m_width = 0.f;
    float m_height = 0.f;
    float m_depth = 0.f;
    
    // World pointer used to detect collisions with world blocks
    World *m_world = nullptr;
};

#endif /* Collidable_hpp */
