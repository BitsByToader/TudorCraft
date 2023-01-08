//
//  Engine.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 27.12.2022.
//

#ifndef Engine_hpp
#define Engine_hpp

#include <vector>
#include <memory>
#include <chrono>

#include "Renderer.hpp"
#include "World.hpp"
#include "Entity.hpp"
#include "PlayerEntity.hpp"

class Engine {
public:
    static Engine* shared();
    
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
    
    void addEntity(float x, float y, float z);

private:
    void checkInput();
    void updateCamera();
    
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
    
    std::vector<std::shared_ptr<Entity>> m_entities;
    Entity* m_cameraBoundEntity = nullptr;
    PlayerEntity* m_player = nullptr;
    
    std::chrono::high_resolution_clock::time_point lastUpdate;
};

#endif /* Engine_hpp */
