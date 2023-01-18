//
//  EntityComponent.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 29.12.2022.
//

#ifndef EntityComponent_hpp
#define EntityComponent_hpp

#include <simd/simd.h>
#include "Math3D.hpp"

/// Defines a component of an entity.
///
/// If an entity wants to be displayed it is required to  have at least one component. This makes the component the primitive that is
/// rendered by the `Renderer` class. There are no restrictions on components, they can have any texture any cuboid shape, and be placed
/// everywhere (relative to its parent entity).
///
/// However, a component also can't be displayed on its own, it has to be owned and managed by its parent entity for it to be useful. As such, it
/// is fully expected that the parent Entity will **completely** manage the life cycle of the component.
class EntityComponent {
public:
    /// Sets the size of the component.
    /// - Parameters:
    ///   - width: New width of component.
    ///   - height: New height of component.
    ///   - depth: New depth of component.
    void setSize(float width, float height, float depth);
    
    /// Sets the textures used for the component.
    /// - Parameter indices: A 6 element array containing the texture indices.
    void setTextures(int *indices);
    
    /// Sets the relative position of the component to the parent entity.
    /// - Parameters:
    ///   - x: New x coordinate.
    ///   - y: New y coordinate.
    ///   - z: New z coordinate.
    void setRelativePosition(float x, float y, float z);
    
    /// Sets the rotataion of the component.
    /// - Parameters:
    ///   - pitch: New pitch angle in radians.
    ///   - yaw: New yaw angle in radians.
    ///   - roll: New roll angle in radians.
    void setRotation(float pitch, float yaw, float roll);
    
    /// Getter for the transform matrix asociated with the component.
    ///
    /// The matrix is relative to the parent, so to get the true matrix, multiply to the left with the parent's transform matrix.
    simd::float4x4 transformMatrix();
    
    /// Returns the rotation of the component stored by axis.
    simd::float3 rotation();
    
private:
    // Generate transform matrix
    void calculateTransform();
    
    // Size
    float m_width = 0.f; // X axis
    float m_height = 0.f; // Y axis
    float m_depth = 0.f; // Z axis
    
    // Textures
    int m_textureIndices[6] = { -1, -1, -1, -1, -1, -1 }; // Indices can be found in Faces enum
    
    // Position, relative to parent
    float m_x = 0.f;
    float m_y = 0.f;
    float m_z = 0.f;
    
    // Rotation
    float m_pitch = 0.f;
    float m_yaw = 0.f;
    float m_roll = 0.f;
    
    // Final transform matrix
    simd::float4x4 m_transformMatrix = matrix_identity_float4x4;
};

#endif /* EntityComponent_hpp */
