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

/// The `Engine` class is the heart of the `TudorCraft` game.
///
/// It is responsible for managing everything from the notifications received from the UI to updating the physics model,
/// to handling entities, to handling the world and rendering everything correctly.
///
/// # Memory management:
///     The engine uses C++11 and later smart pointers to handle all dynamic memory in the engine (aside from the World and the Renderer). This
/// make it very easy to not end up with memory leaks, but more importantly, it ensures a clean implementation for all the sub systems of
/// the engine. As such we don't need to use any complex component based logic to ensure proper ownership, allocation and deallocation of
/// the objects.
///   For example, when we remove an entity, it only necessary for it to be cleared from the  `std::unordered_map` for it to be deallocated
///  along with all of its subcomponents. In this example we can clearly see that we can write the Engine in a very C++ way, i.e. fast, clean
///  and safe.
///   While we currently only support Entities for drawing, MobEntities for physics and collisions, we can easily add other behaviours like AI
///  easily without thinking about what and when to deallocate or if it's truly needed anymore.
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
    
    /// Notifies the engine a mouse button has been pressed.
    /// - Parameter button: The code of the button that was pressed.
    void pressedMouse(int button);
    
    /// Setter for the TCP network client used to communicate with the MC:JE server.
    /// - Parameter s: The server connection.
    void setServerConnection(TCPClient *s);
    
    /// Spawns the player in the world.
    /// - Parameters:
    ///   - x: X render coordinate of spawn point.
    ///   - y: Y render coordinate of spawn point.
    ///   - z: Z render coordinate of spawn point.
    ///   - yaw: Yaw of the player.
    ///   - pitch: Pitch of the player's head.
    void spawnPlayer(float x, float y, float z, float yaw, float pitch);
    
    /// Adds an entity to the world. This can be any entity, like another player, a mob, etc.
    /// - Parameters:
    ///   - id: The id of the entity to spawn.
    ///   - mob: The entity to add to the world.
    void addEntity(int id, std::shared_ptr<MobEntity> mob);
    
    /// Remove the given entity
    /// - Parameter id: ID of entity to remove.
    void removeEntity(int id);
    
    /// Retrieves an entity by its id.
    /// - Parameter id: The id of the entity to retrieve.
    ///
    /// - Returns: A smart pointer of the entity (it's life cycle is automatically managed).
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
    // to prevent general inconsistencies: ghost blocks, latency, etc.
    int m_sequenceCounter = 0;
    
    // The observer that is used to receive notification about hotbar slot changes.
    NS::Object *m_slotNotificationObserver = nullptr;
};

#endif /* Engine_hpp */
