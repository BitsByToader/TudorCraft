//
//  ShaderTypes.h
//  TudorCraft
//
//  Created by Tudor Ifrim on 04.11.2022.
//

#ifndef ShaderTypes_h
#define ShaderTypes_h

#include <simd/simd.h>

#define TEXTURE_COUNT 15

//MARK: - Vertex arguments indices
typedef enum VertexInputIndex {
    VertexInputIndexVertices     = 0,
    VertexInputIndexInstanceData = 1,
    VertexInputIndexCameraData   = 2
} VertexInputIndex;

//MARK: - Texture argument indices
typedef enum TextureIndex {
    TextureIndexBaseColor = 0,
} TextureIndex;

//MARK: - Fragment arguments indices
typedef enum FragmentInputIndex {
    FragmentInputIndexTextures   = 0,
    FragmentInputIndexCameraData = 1
} FragmentInputIndex;

//MARK: - Vertex struct
typedef struct {
    // Position in clipSpace ( the origin is in the center of the view, similar to a graph + depth)
    vector_float3 position;
    
    // Coordinates of the normal vector used for light shading on rotated surfaces
     vector_float3 normal;
    
    // 2D coordinate of texture
    vector_float2 textureCoordinate;
} Vertex;

//MARK: - InstanceData struct
typedef struct InstanceData {
    // Rotation matrix to be applied to the triangle during render
    matrix_float4x4 transform;
    
    // Rotation matrix to be applied to the triangle's normal during render
    matrix_float3x3 normalTransform;
    
    // The index of the texture we want to use from the texture array for the triangle.
    int textureId;
    
    // The block which the triangle belongs to.
    simd_int3 blockCoordinates;
    
    bool highlighted = false;
} InstanceData;

//MARK: - CameraData struct
typedef struct {
    matrix_float4x4 perspectiveTranform;
    matrix_float4x4 worldTranform;
    matrix_float3x3 worldNormalTranform;
} CameraData;

#endif /* ShaderTypes_h */
