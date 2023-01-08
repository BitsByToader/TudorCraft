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

enum Forces: int {
    Gravity = 0,
    Movement = 1,
    Explosion = 2,
    Hit = 3
};

class PhysicsConformable {
public:
    void setDrag(float drag);
    void setVelocity(int key, simd::float3 velocity);
    void removeVelocity(int key);
    simd::float3 calculateDeltaPosition(double deltaTimeMs);
    
private:
    simd::float3 calculateFinalVelocity();
    
    std::unordered_map<int, simd::float3> m_velocities;
    float m_dragMagnitude;
};

#endif /* PhysicsConformable_hpp */
