//
//  Engine.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 27.12.2022.
//

#include "Engine.hpp"

#include "AAPLUtilities.h"

//MARK: - Boilerplate
Engine *Engine::m_sharedObject = nullptr;
Engine* Engine::shared() {
    if ( m_sharedObject == nullptr ) {
        m_sharedObject = new Engine;
    }
    
    return m_sharedObject;
};

Engine::Engine() {
    m_renderer = Renderer::shared();
    m_world = World::shared();
};

Engine::~Engine() {
    // Stub
}

//MARK: - Update loop
void Engine::updateGame(MTL::RenderPassDescriptor *currentRPD, MTL::Drawable* currentDrawable) {
    checkInput();
    
    updateCamera();
    
    m_renderer->draw(m_entities, currentRPD, currentDrawable);
};

void Engine::updateCamera() {
    if ( m_cameraBoundEntity != nullptr ) {
        m_renderer->cameraPosition = m_cameraBoundEntity->cameraPosition();
        m_renderer->cameraYaw = m_cameraBoundEntity->rotation().y;
        m_renderer->cameraPitch = m_cameraBoundEntity->cameraBoundComponent->rotation().x;
    }
};

//MARK: - Input checking
void Engine::checkInput() {
    if ( !m_player )
        return;
    
    simd::float3 pos = m_player->position();
    float pitch = m_player->cameraBoundComponent->rotation().x;
    float yaw = m_player->rotation().y;
    
    // Check mouse
    if ( m_yawRadiansLeft > 0 ) {
        // Right
        if (yaw == -2*M_PI ) {
            yaw = 0;
        } else {
            yaw -= 0.05f;
        }
        
        m_yawRadiansLeft -= 0.5;
    } else if ( m_yawRadiansLeft < 0 ){
        // Left
        if ( yaw == 2*M_PI ) {
            yaw = 0;
        } else {
            yaw += 0.05f;
        }
        
        m_yawRadiansLeft += 0.5;
    }

    if ( m_pitchRadiansLeft < 0 ) {
        // Up
        if ( pitch > -M_PI/2 ) {
            pitch -= 0.05f;
        }
        
        m_pitchRadiansLeft += 0.5;
    } else if ( m_pitchRadiansLeft > 0 ) {
        // Down
        if ( pitch < M_PI/2) {
            pitch += 0.05f;
        }
        
        m_pitchRadiansLeft -= 0.5;
    }
    
    simd::float3 dir = {-2 * cos(pitch) * sin(yaw),
                        -2 * sin(pitch),
                        -2 * cos(pitch) * cos(yaw)};
    bool playerMoved = false;
    
    // Check keyboard
    if ( m_goingForward ) {
        pos += dir;
        
        playerMoved = true;
    }
    
    if ( m_goingLeft ) {
        pos.x += (dir * Math3D::makeYRotate3x3(-M_PI/2)).x;
        pos.z += (dir * Math3D::makeYRotate3x3(-M_PI/2)).z;
        
        playerMoved = true;
    }
    
    if ( m_goingRight ) {
        pos.x += (dir * Math3D::makeYRotate3x3(M_PI/2)).x;
        pos.z += (dir * Math3D::makeYRotate3x3(M_PI/2)).z;
        
        playerMoved = true;
    }
    
    if ( m_goingBackward ) {
        pos -= dir;
        
        playerMoved = true;
    }
    
    if ( m_goingUp ) {
        pos.y += 2.f;
        
        playerMoved = true;
    }
    
    if ( m_goingDown ) {
        pos.y -= 2.f;
        
        playerMoved = true;
    }
    
    if ( playerMoved )
        m_player->setPosition(pos.x, pos.y, pos.z);
    
    simd::float3 temp = m_player->rotation();
    m_player->setRotation(temp.x, yaw);
    
    temp = m_player->cameraBoundComponent->rotation();
    m_player->cameraBoundComponent->setRotation(pitch, temp.y, temp.z);
};

void Engine::keyDown(int16_t code) {
    switch (code) {
        case 13: // W
            m_goingForward = true;
            break;
            
        case 0: // A
            m_goingLeft = true;
            break;
            
        case 1: // S
            m_goingBackward = true;
            break;
            
        case 2: // D
            m_goingRight = true;
            break;
            
        case 48: // Tab
            m_goingDown = true;
            break;
            
        case 49: // Space
            m_goingUp = true;
            break;
            
        default:
            break;
    }
};

void Engine::keyUp(int16_t code) {
    switch (code) {
        case 13: // W
            m_goingForward = false;
            break;
            
        case 0: // A
            m_goingLeft = false;
            break;
            
        case 1: // S
            m_goingBackward = false;
            break;
            
        case 2: // D
            m_goingRight = false;
            break;
            
        case 48: // Tab
            m_goingDown = false;
            break;
            
        case 49: // Space
            m_goingUp = false;
            break;
            
        default:
            break;
    }
};

void Engine::mouseMoved(float x, float y) {
    m_yawRadiansLeft = x;
    m_pitchRadiansLeft = y;
};

void Engine::addEntity(float x, float y, float z) {
    
    std::shared_ptr<PlayerEntity> player = std::make_shared<PlayerEntity>(x, y, z);
    
    m_entities.push_back(player);
    m_cameraBoundEntity = player.get();
    m_player = player.get();
};
