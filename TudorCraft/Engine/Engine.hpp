//
//  Engine.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 27.12.2022.
//

#ifndef Engine_hpp
#define Engine_hpp

#include "Renderer.hpp"
#include "World.hpp"

class Engine {
public:
    static Engine* shared();
    
    Engine();
    
    void updateGame();
    
    void keyDown(int16_t code);
    void keyUp(int16_t code);
    void mouseMoved(float x, float y);

private:
    void checkInput();
    
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
};

#endif /* Engine_hpp */
