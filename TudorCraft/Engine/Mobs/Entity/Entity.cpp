//
//  Entity.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 29.12.2022.
//

#include "Entity.hpp"

Entity::~Entity() {
    for ( auto i: m_components ) {
        delete i;
    }
    m_components.clear();
};

void Entity::setPosition(float x, float y, float z) {
    m_x = x;
    m_y = y;
    m_z = z;
    
    calculateTransform();
};

void Entity::setRotation(float pitch, float yaw) {
    m_pitch = pitch;
    m_yaw = yaw;
    
    calculateTransform();
};

void Entity::addComponent(EntityComponent *c) {
    m_components.push_back(c);
};

simd::float4x4 Entity::transformMatrix() {
    return m_transformMatrix;
};

std::vector<EntityComponent *> Entity::components() {
    return m_components;
};

simd::float3 Entity::cameraPosition() {
    return { m_x + m_cameraX, m_y + m_cameraY, m_z + m_cameraZ };
};

simd::float3 Entity::position() {
    return { m_x, m_y, m_z };
};

simd::float3 Entity::rotation() {
    return { m_pitch, m_yaw, 0.f};
};

void Entity::calculateTransform() {
    using namespace simd;
    
    float4x4 translateMatrix = Math3D::makeTranslate({ m_x, m_y, m_z });
    float4x4 rotationXMatrix = Math3D::makeXRotate4x4(m_pitch);
    float4x4 rotationYMatrix = Math3D::makeYRotate4x4(m_yaw);
    
    m_transformMatrix = translateMatrix * rotationXMatrix * rotationYMatrix;
};
