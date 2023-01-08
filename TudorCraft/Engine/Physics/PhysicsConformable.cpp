//
//  PhysicsConformable.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 07.01.2023.
//

#include "PhysicsConformable.hpp"

void PhysicsConformable::setDrag(float drag) {
    m_dragMagnitude = drag;
};

void PhysicsConformable::setVelocity(int key, simd::float3 velocity) {
    m_velocities[key] = velocity;
};

void PhysicsConformable::removeVelocity(int key) {
    m_velocities.erase(key);
};

simd::float3 PhysicsConformable::calculateFinalVelocity() {
    simd::float3 finalV = { 0.f, 0.f, 0.f };
    
    int sign = 1;
    for ( auto &i: m_velocities ) {
        if ( i.second.x == 0 && i.second.y == 0 && i.second.z == 0 ) {
            continue;
        }
        
        // Calculate drag for velocity
        simd::float3 drag = simd::normalize(i.second);
        drag *= m_dragMagnitude;
        
        // Check if we're chaning direction
        float copyX = i.second.x;
        sign = copyX < 0 ? -1 : 1;
        copyX *= sign; // Make the component positive
        copyX -= (drag.x < 0 ? -drag.x : drag.x); // Apply drag
        if ( copyX < 0 ) {
            i.second = { 0.f, 0.f, 0.f };
        } else {
            // Apply drag
            i.second -= drag;
        }
        
        finalV += i.second;
    }
    
    return finalV;
};

simd::float3 PhysicsConformable::calculateDeltaPosition(double deltaTimeMs) {
    double timeS = deltaTimeMs / 1000;
    
    // Why the 10 at the end? Velocities are measured in meters per second.
    // For us, a meter is 10.f, so we need to convert from real units to ingame rendering units.
    simd::float3 pos = calculateFinalVelocity() * timeS * 10;
    
    return pos;
};

