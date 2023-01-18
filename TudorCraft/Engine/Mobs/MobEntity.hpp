//
//  PlayerEntity.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 03.01.2023.
//

#ifndef PlayerEntity_hpp
#define PlayerEntity_hpp

#include "Entity.hpp"
#include "Collidable.hpp"
#include "PhysicsConformable.hpp"

/// The actual usefull class of all the three that it inherits from: `Entity`, `Collidable` and `PhysicsConformable`.
///
/// This class defines the behaviour of every component in the game and can be inherited from to create complex entities with rich
/// behaviours and looks.
///
/// Note: Since the game doesn't support any other entities, it is the one creates all of the entities in-game, however this should be an
/// abstract class that is only inherited from, since it shouldn't have any creation logic.
class MobEntity: public Entity, public Collidable, public PhysicsConformable {
public:
    MobEntity(float x, float y, float z);
    
    /// This method puts toghether all of the methods from the three  classes it inherits form. It take the time to calculate a new position
    /// using the physics model, then it checks for collisions updating the position to the correct one, and finally it sets the new position
    /// visually for it to be rendered.
    /// - Parameter timeMs: The time it takes for the movement to happen.
    void updatePosition(double timeMs);
    
    /// Sets the movement speed of the MobEntity.
    /// - Parameters:
    ///   - x: Speed in the X-Axis.
    ///   - y: Speed in the Y-Axis.
    ///   - z: Speed in the Z-Axis.
    void setSpeed(float x, float y, float z);

    /// Sets whether or not the MobEntity is on ground. Used when checking if movement should be applied to the entity and when sending the
    /// position to the server.
    bool onGround = false;
};

#endif /* PlayerEntity_hpp */
