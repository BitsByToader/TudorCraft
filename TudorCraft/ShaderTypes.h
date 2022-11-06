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
    VertexInputIndexViewportSize = 1,
} VertexInputIndex;

typedef enum TextureIndex {
    TextureIndexBaseColor = 0,
} TextureIndex;

typedef struct {
    // Position in clipSpace ( the origin is in the center of the view, similar to a graph + depth)
    vector_float3 position;
    
    // 2D coordinate of texture
    vector_float2 textureCoordinate;
} Vertex;

#endif /* ShaderTypes_h */
