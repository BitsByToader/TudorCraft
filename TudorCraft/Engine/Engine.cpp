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

void Engine::destroySharedObject() {
    delete m_sharedObject;
    m_sharedObject = nullptr;
};

Engine::Engine() {
    m_renderer = Renderer::shared();
    m_world = World::shared();
    
    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();
    NS::String *name = NS::String::alloc()->init("slot", NS::StringEncoding::UTF8StringEncoding);
    
    m_slotNotificationObserver = NS::NotificationCenter::defaultCenter()->addObserver(name,
                                                         NULL, NULL,
                                                         ^void( NS::Notification *notif ) {
        NS::Number *received = (NS::Number*) notif->object();
        int slot = received->intValue();
        selectHeldItem(slot);
    });
    m_slotNotificationObserver->retain();
    
    pool->release();
    
    lastUpdate = std::chrono::high_resolution_clock::now();
};

Engine::~Engine() {
    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();
    NS::NotificationCenter::defaultCenter()->removeObserver(m_slotNotificationObserver);
    m_slotNotificationObserver->release();
    pool->release();
}

//MARK: - Update loop
void Engine::updateGame(MTL::RenderPassDescriptor *currentRPD, MTL::Drawable* currentDrawable) {
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> passed = now - lastUpdate;
    
    for(auto &i: m_entities) {
        i.second->updatePosition(passed.count());
    }
    
    if ( m_player ) {
#warning This packet should normally be sent every 50ms (a server tick), but this works as well.
        MCP::SetPlayerPositionAndRotation packet;
        packet.m_x = m_player->position().x / 10;
        packet.m_y = m_player->position().y / 10;
        packet.m_z = -m_player->position().z / 10;
        packet.m_yaw = -m_player->rotation().y / M_PI * 180.f - 90.f;
        packet.m_pitch = m_player->cameraBoundComponent->rotation().x / M_PI * 180.f;
        packet.m_onGround = m_player->onGround;
        
        m_serverConnection->sendPacket(&packet);
    }
    
    lastUpdate = now;
    
    checkInput();
    
    updateCamera();
    
    updateLookTarget();
    
    std::vector<std::shared_ptr<Entity>> entities;
    for ( auto &i: m_entities ) {
        entities.push_back(i.second);
    }
    
    m_renderer->draw(entities, currentRPD, currentDrawable);
};

void Engine::updateLookTarget() {
    if ( !m_player)
        return;
    
    float pitch = m_player->cameraBoundComponent->rotation().x;
    float yaw = m_player->rotation().y;
    simd::float3 rayPosition = m_player->cameraPosition();
    
    simd::float3 lookDirection = {-cos(pitch) * sin(yaw),
                                  -sin(pitch),
                                  -cos(pitch) * cos(yaw)};
    
    for ( float i = 0; i < 50; i += 0.1 ) {
        // Only highlight a block if it isn't more than 5 block away
        simd::int3 blockToCheck = { (int)rayPosition.x / 10,
            (int)rayPosition.y / 10,
            (int)-rayPosition.z / 10
        };
        
        Block *b = m_world->getBlockAt(blockToCheck.x, blockToCheck.y, blockToCheck.z);
        if ( b != nullptr && b->state != nullptr ) {
            // We found a non air block to highlight!
            if ( m_highlightedBlockCoords.x != blockToCheck.x ||
                m_highlightedBlockCoords.y != blockToCheck.y ||
                m_highlightedBlockCoords.z != blockToCheck.z)
            {
                m_world->highlightBlock(blockToCheck.x, blockToCheck.y, blockToCheck.z);
                m_highlightedBlockCoords = blockToCheck;
                m_raycastHitPosition = rayPosition;
            }
            
            break;
        }
        
        // Didn't find block to highlight
//        m_world->removeHightlight();
        
        // Go through the entities here and check their position when implementing hits.
        // Important to check after the blocks as to first remove a block and then hit the entity behind it.
        
        rayPosition += lookDirection * 0.1;
    }
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
    
    simd::float3 moveDirection = {-7.5f * sin(yaw),
                                  0.f,
                                  -7.5f * cos(yaw)};
    
    simd::float3 movementVector = { 0.f, 0.f, 0.f };
    
    bool playerMoved = false;
    
    // Check keyboard
    if ( m_goingForward ) {
        movementVector.x += moveDirection.x;
        movementVector.z += moveDirection.z;
        
        playerMoved = true;
    }
    
    if ( m_goingLeft ) {
        movementVector.x += (moveDirection * Math3D::makeYRotate3x3(-M_PI/2)).x;
        movementVector.z += (moveDirection * Math3D::makeYRotate3x3(-M_PI/2)).z;
        
        playerMoved = true;
    }
    
    if ( m_goingRight ) {
        movementVector.x += (moveDirection * Math3D::makeYRotate3x3(M_PI/2)).x;
        movementVector.z += (moveDirection * Math3D::makeYRotate3x3(M_PI/2)).z;
        
        playerMoved = true;
    }
    
    if ( m_goingBackward ) {
        movementVector.x -= moveDirection.x;
        movementVector.z -= moveDirection.z;
        
        playerMoved = true;
    }
    
    if ( m_goingUp ) {
        movementVector.y += 20.f;
        
        playerMoved = true;
    }
    
    if ( playerMoved && m_player->onGround )
        m_player->setSpeed(movementVector.x, movementVector.y, movementVector.z);
    
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
    int newSlot = -1;
    
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
            
        case 18: // 1
            newSlot = 0;
            break;
        
        case 19: // 2
            newSlot = 1;
            break;
        case 20: // 3
            newSlot = 2;
            break;
        case 21: // 4
            newSlot = 3;
            break;
        case 23: // 5
            newSlot = 4;
            break;
        case 22: // 6
            newSlot = 5;
            break;
        case 26: // 7
            newSlot = 6;
            break;
        case 28: // 8
            newSlot = 7;
            break;
        case 25: // 9
            newSlot = 8;
            break;
            
        default:
            break;
    }
    
    if ( newSlot != -1 ) {
        // Send new held slot to UI and Engine via notifications
        NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();
        
        NS::Number *number = NS::Number::alloc()->init(newSlot);
        NS::String *name = NS::String::alloc()->init("slot", NS::UTF8StringEncoding);
        NS::NotificationCenter::defaultCenter()->postNotificationName(name, number);
        
        number->release();
        name->release();
        pool->release();
    }
};

void Engine::mouseMoved(float x, float y) {
    m_yawRadiansLeft = x;
    m_pitchRadiansLeft = y;
};

void Engine::pressedMouse(int button) {
    m_renderer->m_gpuMutex.lock();
    // 0 - left
    // 1 - right
    if ( button == 0 ) {
        // Remove block
        m_world->placeBlockAt(m_highlightedBlockCoords.x,
                              m_highlightedBlockCoords.y,
                              m_highlightedBlockCoords.z, nullptr);
        
        MCP::PlayerActionPacket startdig(m_highlightedBlockCoords.x,
                                       m_highlightedBlockCoords.y,
                                       m_highlightedBlockCoords.z,
                                       m_sequenceCounter);
        m_serverConnection->sendPacket(&startdig);
    } else if ( button == 1 ) {
        // Place block
        simd::float3 lookDirection = {
            m_highlightedBlockCoords.x + .5f,
            m_highlightedBlockCoords.y + .5f,
            m_highlightedBlockCoords.z + .5f,
        };
        lookDirection -= m_raycastHitPosition;
        
        const simd::float3 vecRight = {1.f, 0.f, 0.f};
        const simd::float3 vecUp = {0.f, 1.f, 0.f};
        const simd::float3 vecForward = {0.f, 0.f, 1.f};
        
        float up = Math3D::cosBetween(lookDirection, vecUp);
        float down = -up;
        float right = Math3D::cosBetween(lookDirection, vecRight);
        float left = -right;
        float forward = Math3D::cosBetween(lookDirection, vecForward);
        float backward = -forward;
        
        float min = std::min({up, down, right, left, forward, backward});
        
        int face = 0;
        if ( min == up ) {
            // Place on top
            m_world->placeBlockAt(m_highlightedBlockCoords.x,
                                  m_highlightedBlockCoords.y+1,
                                  m_highlightedBlockCoords.z, BlockState::GrassBlock());
            face = 1;
            
        } else if ( min == down ) {
            // Place on bottom
            m_world->placeBlockAt(m_highlightedBlockCoords.x,
                                  m_highlightedBlockCoords.y-1,
                                  m_highlightedBlockCoords.z, BlockState::GrassBlock());
            face = 0;
            
        } else if ( min == left ) {
            // Place to the left
            m_world->placeBlockAt(m_highlightedBlockCoords.x-1,
                                  m_highlightedBlockCoords.y,
                                  m_highlightedBlockCoords.z, BlockState::GrassBlock());
            face = 4;
            
        } else if ( min == right ) {
            // Place to the right
            m_world->placeBlockAt(m_highlightedBlockCoords.x+1,
                                  m_highlightedBlockCoords.y,
                                  m_highlightedBlockCoords.z, BlockState::GrassBlock());
            face = 5;
            
        } else if ( min == forward ) {
            // Place in front
            m_world->placeBlockAt(m_highlightedBlockCoords.x,
                                  m_highlightedBlockCoords.y,
                                  m_highlightedBlockCoords.z-1, BlockState::GrassBlock());
            face = 2;
            
        } else if ( min == backward ) {
            // Place in the back
            m_world->placeBlockAt(m_highlightedBlockCoords.x,
                                  m_highlightedBlockCoords.y,
                                  m_highlightedBlockCoords.z+1, BlockState::GrassBlock());
            face = 3;
            
        }
        
        m_world->removeHightlight();
        
        MCP::UseItemOnPacket place(m_highlightedBlockCoords.x,
                                   m_highlightedBlockCoords.y,
                                   m_highlightedBlockCoords.z,
                                   face,
                                   m_sequenceCounter);
        m_serverConnection->sendPacket(&place);
    }
    m_renderer->m_gpuMutex.unlock();
    m_sequenceCounter++;
};

// MARK: - Entity Management
void Engine::spawnPlayer(float x, float y, float z, float yaw, float pitch) {
    std::shared_ptr<MobEntity> player = std::make_shared<MobEntity>(x, y, z);
    
    simd::float3 temp = player->rotation();
    player->setRotation(temp.x, yaw);

    temp = player->cameraBoundComponent->rotation();
    player->cameraBoundComponent->setRotation(pitch, temp.y, temp.z);
    
    m_entities[0] = player;
    m_cameraBoundEntity = player.get();
    m_player = player.get();
};

void Engine::addEntity(int id, std::shared_ptr<MobEntity> mob) {
    m_entities[id] = mob;
};

void Engine::removeEntity(int id) {
    m_entities.erase(id);
};

std::shared_ptr<MobEntity> Engine::getEntity(int id) {
    try {
        return m_entities.at(id);
    } catch(...) {
        //
    };
    
    return std::shared_ptr<MobEntity>();
};

// MARK: - Networking
void Engine::setServerConnection(TCPClient *s) {
    m_serverConnection = s;
};

void Engine::selectHeldItem(int slotNumber) {
    // Send new held item to server
    MCP::SetHeldItemPacket p(slotNumber);
    m_serverConnection->sendPacket(&p);
};
