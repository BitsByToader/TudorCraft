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
    float4 position [[position]];
    
    float3 normal;

    // Since this member does not have a special attribute qualifier, the rasterizer
    // will interpolate its value with values of other vertices making up the triangle
    // and pass that interpolated value to the fragment shader for each fragment in
    // that triangle.
    float2 textureCoordinate;
};

vertex RasterizerData
vertexShader(uint vertexID [[vertex_id]],
             uint instanceId [[instance_id]],
             constant Vertex *vertices [[buffer(VertexInputIndexVertices)]],
             constant InstanceData *instanceData [[buffer(VertexInputIndexInstanceData)]],
             constant CameraData &cameraData [[buffer(VertexInputIndexCameraData)]])
{
    RasterizerData out;
    
    simd::float4 pos;
    pos.x = vertices[vertexID].position.x;
    pos.y = vertices[vertexID].position.y;
    pos.z = vertices[vertexID].position.z;
    pos.w = 1.0f;
    
    pos = instanceData[instanceId].transform * pos;
    pos = cameraData.perspectiveTranform * cameraData.worldTranform * pos;
    out.position = pos;
    
    float3 normal = instanceData[instanceId].normalTransform * vertices[vertexID].normal;
    normal = cameraData.worldNormalTranform * normal;
    out.normal = normal;
    
    out.textureCoordinate = vertices[vertexID].textureCoordinate;

    return out;
}

// Fragment function
fragment float4
samplingShader(RasterizerData in [[stage_in]],
               texture2d<half> colorTexture [[ texture(TextureIndexBaseColor) ]],
               constant CameraData *cameraData [[buffer(VertexInputIndexCameraData)]])
{
    constexpr sampler textureSampler (mag_filter::nearest, min_filter::linear);

    // Sample the texture to obtain a color
    const half4 colorSample = colorTexture.sample(textureSampler, in.textureCoordinate);
    
    // TODO: Tranform the light coordinates along with the world coordinates to get proper shading
    
    // assume light coming from (front-top-right)
    float3 l = normalize(cameraData->worldNormalTranform * float3( 1.0, 1.0, 1.0 ));
    float3 n = normalize( in.normal );

    float ndotl = saturate( dot( n, l ) );

    // return the color of the texture
    return float4(colorSample * 0.1 + colorSample * ndotl);
}


