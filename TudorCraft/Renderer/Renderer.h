//
//  Renderer.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 02.11.2022.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include <Metal/Metal.hpp>
#include <simd/simd.h>

#include "TextureAtlas.hpp"

#define MAX_FRAMES_IN_FLIGHT 3

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    void loadMetal();
    
    MTL::Texture *loadTextureUsingAtlas();
    
    void windowSizeWillChange(unsigned int width, unsigned int height);
    
    void draw(MTL::RenderPassDescriptor *currentRPD, MTL::Drawable* currentDrawable);
    
    void forward();
    void backward();
    void left();
    void right();
    void up();
    void down();
    
    void lookUp();
    void lookDown();
    void lookRight();
    void lookLeft();

private:
    dispatch_semaphore_t m_inFlightSemaphore;
    
    /// The device we're using to render.
    MTL::Device* m_device;
    
    /// The command queue used to pass commands to the device.
    MTL::CommandQueue* m_commandQueue;
    
    /// The render pipeline generated from the vertex and fragment shaders
    MTL::RenderPipelineState* m_pipelineState;
    
    /// Combined depth and stenctil object
    MTL::DepthStencilState* m_depthState;
    
    /// Metal texture buffer
    MTL::Texture *m_texture;
    
    /// Vertex data buffer
    MTL::Buffer *m_vertices;
    
    /// Indices buffer
    MTL::Buffer* m_indexBuffer;
    
    /// Instance data buffer
    MTL::Buffer* m_instanceDataBuffers[MAX_FRAMES_IN_FLIGHT];
    
    /// Camera data buffer
    MTL::Buffer* m_cameraDataBuffer;
    
    /// Number of vertices
    NS::UInteger m_verticesCount;
    
    /// Window size
    vector_uint2 m_windowSize;
    
    /// The atlas we're using to load the textures from.
    TextureAtlas *m_atlas;
    
    /// Semaphore used for synchronizing the CPU and GPU
    dispatch_semaphore_t m_semaphore;
    
    /// Frame we're currently working on
    int m_frame;
    
    float m_yawAngle = 0.f;
    float m_pitchAngle = 0.f;
    
    simd::float3 m_playerPos = (simd::float3) { 0.f, 0.f, 0.0f };
    
    int recalculateBlocks = MAX_FRAMES_IN_FLIGHT;
};

#endif /* Renderer_hpp */
