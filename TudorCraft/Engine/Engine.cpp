//
//  Engine.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 27.12.2022.
//

#include "Engine.hpp"

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

void Engine::updateGame() {
    checkInput();
};

void Engine::checkInput() {
    // Check keyboard
    if ( m_goingForward ) {
        m_renderer->forward();
    }
    
    if ( m_goingLeft ) {
        m_renderer->left();
    }
    
    if ( m_goingRight ) {
        m_renderer->right();
    }
    
    if ( m_goingBackward ) {
        m_renderer->backward();
    }
    
    if ( m_goingUp ) {
        m_renderer->up();
    }
    
    if ( m_goingDown ) {
        m_renderer->down();
    }
    
    // Check mouse
    if ( m_yawRadiansLeft > 0 ) {
        m_renderer->lookRight();
        m_yawRadiansLeft -= 0.5;
    } else if ( m_yawRadiansLeft < 0 ){
        m_renderer->lookLeft();
        m_yawRadiansLeft += 0.5;
    }

    if ( m_pitchRadiansLeft < 0 ) {
        m_renderer->lookUp();
        m_pitchRadiansLeft += 0.5;
    } else if ( m_pitchRadiansLeft > 0 ) {
        m_renderer->lookDown();
        m_pitchRadiansLeft -= 0.5;
    }
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
