//
//  Shaders.metal
//  TudorCraft
//
//  Created by Tudor Ifrim on 04.11.2022.
//

#include <metal_stdlib>

using namespace metal;

// Include header shared between this Metal shader code and C code executing Metal API commands.
#include "ShaderTypes.h"

// Vertex shader outputs and fragment shader inputs
struct RasterizerData {
    // The [[position]] attribute qualifier of this member indicates this value is
    // the clip space position of the vertex when this structure is returned from
    // the vertex shader
    float4 clipSpacePosition [[position]];

    // Since this member does not have a special attribute qualifier, the rasterizer
    // will interpolate its value with values of other vertices making up the triangle
    // and pass that interpolated value to the fragment shader for each fragment in
    // that triangle.
    float2 textureCoordinate;
};

vertex RasterizerData
vertexShader(uint vertexID [[vertex_id]],
             constant Vertex *vertices [[buffer(VertexInputIndexVertices)]],
             constant vector_uint2 *viewportSizePointer [[buffer(VertexInputIndexViewportSize)]])
{
    RasterizerData out;

    // Index into the array of positions to get the current vertex.
    // The positions are specified in pixel dimensions (i.e. a value of 100
    // is 100 pixels from the origin).
    float2 pixelSpacePosition = float2(vertices[vertexID].position.xy);

    // Get the viewport size and cast to float.
    vector_float2 viewportSize = vector_float2(*viewportSizePointer);
    
    // Initialize the output clip-space position.
    out.clipSpacePosition = vector_float4(0.0, 0.0, 0.0, 1.0);

    // To convert from positions in pixel space to positions in clip-space,
    //  divide the pixel coordinates by half the size of the viewport.
    out.clipSpacePosition.xy = (pixelSpacePosition / (viewportSize / 2.0));
    out.clipSpacePosition.z = vertices[vertexID].position.z;

    // Pass the input color directly to the rasterizer.
    out.textureCoordinate = vertices[vertexID].textureCoordinate;

    return out;
}

// Fragment function
fragment float4
samplingShader(RasterizerData in [[stage_in]],
               texture2d<half> colorTexture [[ texture(TextureIndexBaseColor) ]])
{
    constexpr sampler textureSampler (mag_filter::linear,
                                      min_filter::linear);

    // Sample the texture to obtain a color
    const half4 colorSample = colorTexture.sample(textureSampler, in.textureCoordinate);

    // return the color of the texture
    return float4(colorSample);
}


