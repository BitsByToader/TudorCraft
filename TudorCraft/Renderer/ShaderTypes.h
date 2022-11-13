//
//  ShaderTypes.h
//  TudorCraft
//
//  Created by Tudor Ifrim on 04.11.2022.
//

#ifndef ShaderTypes_h
#define ShaderTypes_h

#include <simd/simd.h>

typedef enum VertexInputIndex {
    VertexInputIndexVertices     = 0,
    VertexInputIndexInstanceData = 1,
    VertexInputIndexCameraData = 2
} VertexInputIndex;

typedef enum TextureIndex {
    TextureIndexBaseColor = 0,
} TextureIndex;

typedef struct {
    // Position in clipSpace ( the origin is in the center of the view, similar to a graph + depth)
    vector_float3 position;
    
    // Coordinates of the normal vector used for light shading on rotated surfaces
     vector_float3 normal;
    
    // 2D coordinate of texture
    vector_float2 textureCoordinate;
} Vertex;

typedef struct {
    // Rotation matrix to be applied to the triangle during render
    matrix_float4x4 transform;
    
    // Rotation matrix to be applied to the triangle's normal during render
    matrix_float3x3 normalTransform;
} InstanceData;

typedef struct {
    matrix_float4x4 perspectiveTranform;
    matrix_float4x4 worldTranform;
    matrix_float3x3 worldNormalTranform;
} CameraData;

#endif /* ShaderTypes_h */
