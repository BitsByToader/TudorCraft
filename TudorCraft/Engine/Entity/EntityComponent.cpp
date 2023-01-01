//
//  EntityComponent.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 29.12.2022.
//

#include "EntityComponent.hpp"


void EntityComponent::setSize(float width, float height, float depth) { 
    m_width = width;
    m_height = height;
    m_depth = depth;
    
    calculateTransform();
}

void EntityComponent::setTextures(int *indices) { 
    for ( int i = 0; i < 6; i++ ) {
        m_textureIndices[i] = indices[i];
    };
}

void EntityComponent::setRelativePosition(float x, float y, float z) { 
    m_x = x;
    m_y = y;
    m_z = z;
    
    calculateTransform();
}

void EntityComponent::setRotation(float pitch, float yaw, float roll) { 
    m_pitch = pitch;
    m_yaw = yaw;
    m_roll = roll;
    
    calculateTransform();
}

simd::float4x4 EntityComponent::transformMatrix() {
    return m_transformMatrix;
};

simd::float3 EntityComponent::rotation() {
    return { m_pitch, m_yaw, m_roll };
};

void EntityComponent::calculateTransform() {
    using namespace simd;
    
    float4x4 scaleMatrix = Math3D::makeScale({ m_width, m_height, m_depth });
    float4x4 translateMatrix = Math3D::makeTranslate({ m_x, m_y, m_z });
    float4x4 rotationXMatrix = Math3D::makeXRotate4x4(m_pitch);
    float4x4 rotationYMatrix = Math3D::makeYRotate4x4(m_yaw);
    float4x4 rotationZMatrix = Math3D::makeZRotate4x4(m_roll);
    
    m_transformMatrix = translateMatrix *
                        scaleMatrix *
                        rotationXMatrix * rotationYMatrix * rotationZMatrix;
};



