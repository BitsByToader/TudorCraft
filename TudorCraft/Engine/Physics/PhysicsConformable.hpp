//
//  PhysicsConformable.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 07.01.2023.
//

#ifndef PhysicsConformable_hpp
#define PhysicsConformable_hpp

#include <unordered_map>
#include <simd/simd.h>
#include "AAPLUtilities.h"

/// Forces enum mapping the the forces which will be applied to the entities.
enum Forces: int {
    Gravity = 0,
    Movement = 1,
    Explosion = 2,
    Hit = 3
};

/// Abstract class that is responsible for applying the physics model to any object inheriting it.
///
/// The physics model used is a simple one, as it only supports velocities being applied to objects. The class is used to keep track
/// of all of the velocities of the object, and calculate the resultant velocity and its position relative to its last postion in a given timeframe.
class PhysicsConformable {
public:
    /// Sets the drag of the object.
    /// - Parameter drag: Drag of object.
    void setDrag(float drag);
    
    /// Sets one of the velocities of the object, as they are mapped in the `Forces` enum.
    /// - Parameters:
    ///   - key: The velocity being applied
    ///   - velocity: The value of the velocity.
    void setVelocity(int key, simd::float3 velocity);
    
    /// Removes the selected velocity.
    /// - Parameter key: The velocity to remove.
    void removeVelocity(int key);
    
    /// Calculate the distance the object would move in the given time frame.
    /// - Parameter deltaTimeMs: The time frame of the movement.
    simd::float3 calculateDeltaPosition(double deltaTimeMs);
    
private:
    simd::float3 calculateFinalVelocity();
    
    std::unordered_map<int, simd::float3> m_velocities;
    float m_dragMagnitude;
};

#endif /* PhysicsConformable_hpp */
