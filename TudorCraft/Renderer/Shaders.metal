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
    float4 position [[position]];
    float3 normal;
    float2 textureCoordinate;
    int textureId;
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
    pos.xyz = vertices[vertexID].position.xyz;
    pos.w = 1.0f;
    
    pos = instanceData[instanceId].transform * pos;
    pos = cameraData.perspectiveTranform * cameraData.worldTranform * pos;
    out.position = pos;
    
    float3 normal = instanceData[instanceId].normalTransform * vertices[vertexID].normal;
    normal = cameraData.worldNormalTranform * normal;
    out.normal = normal;
    
    out.textureCoordinate = vertices[vertexID].textureCoordinate;
    out.textureId = instanceData[instanceId].textureId;

    return out;
}

struct FragmentShaderTextures {
    array<texture2d<half>, 3> arr [[ id(TextureIndexBaseColor) ]];
};

// Fragment function
fragment float4
samplingShader(RasterizerData in [[stage_in]],
               device FragmentShaderTextures &fragmentTextures [[buffer(FragmentInputIndexTextures)]],
               constant CameraData *cameraData [[buffer(FragmentInputIndexCameraData)]])
{
    constexpr sampler textureSampler (mag_filter::nearest, min_filter::linear);

    // Sample the texture to obtain a color
    const half4 colorSample = fragmentTextures.arr[in.textureId].sample(textureSampler, in.textureCoordinate);
    
    // TODO: Tranform the light coordinates along with the world coordinates to get proper shading
    // assume light coming from (front-top-right)
    float3 l = normalize(cameraData->worldNormalTranform * float3( 1.0, 1.0, 1.0 ));
    float3 n = normalize( in.normal );

    float ndotl = saturate( dot( n, l ) );

    // return the color of the texture
    return float4(colorSample * 0.2 + colorSample * ndotl);
}


