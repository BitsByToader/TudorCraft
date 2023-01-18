//
//  Engine.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 27.12.2022.
//

#ifndef Engine_hpp
#define Engine_hpp

#include <vector>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <algorithm>

#include <Foundation/Foundation.hpp>

#include "Math3D.hpp"
#include "Renderer.hpp"
#include "World.hpp"
#include "Chunk.hpp"
#include "Entity.hpp"
#include "MobEntity.hpp"
#include "TCPClient.hpp"
#include "Packet.hpp"

class Engine {
public:
    static Engine* shared();
    static void destroySharedObject();
    
    Engine();
    ~Engine();
    
    /// Mainloop of the engine.
    void updateGame(MTL::RenderPassDescriptor *currentRPD, MTL::Drawable* currentDrawable);
    
    /// Notifies the engine a key has been pressed.
    /// - Parameter code: The key code associated with the event.
    void keyDown(int16_t code);
    
    /// Notifies the engine a key has been released.
    /// - Parameter code: The key code associated with the event.
    void keyUp(int16_t code);
    
    /// Notifies the engine the mouse has moved.
    /// - Parameters:
    ///   - x: New relative (to the last position) x coordinate of the mouse.
    ///   - y: New relative (to the last position) y coordinate of the mouse.
    void mouseMoved(float x, float y);
    
    void pressedMouse(int button);
    
    void setServerConnection(TCPClient *s);
    
    void spawnPlayer(float x, float y, float z, float yaw, float pitch);
    void addEntity(int id, std::shared_ptr<MobEntity> mob);
    void removeEntity(int id);
    std::shared_ptr<MobEntity> getEntity(int id);
    
private:
    void checkInput();
    void updateCamera();
    void updateLookTarget();
    void selectHeldItem(int slotNumber);
    
    // Global object
    static Engine *m_sharedObject;
    
    // Game related singletons
    Renderer *m_renderer = nullptr;
    World *m_world = nullptr;
    
    // Key related booleans
    bool m_goingForward = false;
    bool m_goingBackward = false;
    bool m_goingLeft = false;
    bool m_goingRight = false;
    bool m_goingUp = false;
    bool m_goingDown = false;
    
    // Indicates whether or not we have turns to do
    float m_yawRadiansLeft = 0;
    float m_pitchRadiansLeft =  0;
    
    // Entities
    std::unordered_map<int, std::shared_ptr<MobEntity>> m_entities;
    Entity* m_cameraBoundEntity = nullptr;
    MobEntity* m_player = nullptr;
    
    // Time keeping
    std::chrono::high_resolution_clock::time_point lastUpdate;
    
    // Highlighting
    simd::int3 m_highlightedBlockCoords = {0, 0, 0};
    simd::float3 m_raycastHitPosition = {0.f, 0.f, 0.f};
    
    // Networking
    TCPClient *m_serverConnection = nullptr;
    // Counter used by the server to keep track of player actions (place, remove block, digging)
    // to prevent ghost blocks, latency, etc.
    int m_sequenceCounter = 0;
    
    NS::Object *m_slotNotificationObserver = nullptr;
};

#endif /* Engine_hpp */
