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
#include "ShaderTypes.h"
#include "TextureAtlas.hpp"

#define MAX_FRAMES_IN_FLIGHT 3

class Renderer {
public:
    // MARK: Public Methods
    Renderer();
    ~Renderer();
    
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
    // MARK: Private methods
    void loadMetal();
    void createHeap();
    void moveResourcesToHeap();
    static MTL::TextureDescriptor *newDescriptorFromTexture(MTL::Texture *texture,
                                                            MTL::StorageMode storageMode);
    
    int calculateMeshes(InstanceData *instanceData);
    
    // MARK: Private members
    /// The device we're using to render.
    MTL::Device* m_device;
    
    /// The command queue used to pass commands to the device.
    MTL::CommandQueue* m_commandQueue;
    
    /// The render pipeline generated from the vertex and fragment shaders
    MTL::RenderPipelineState* m_pipelineState;
    
    /// Combined depth and stenctil object
    MTL::DepthStencilState* m_depthState;
    
    /// The heap where all the GPU resources will reside in
    MTL::Heap *m_heap;
    
    /// Metal textures buffer
    MTL::Texture *m_texture[3];
    int m_textureCount = 3;
    
    /// Vertex data buffer
    MTL::Buffer *m_vertices;
    
    /// Indices buffer
    MTL::Buffer* m_indexBuffer;
    
    /// Instance data buffer
    MTL::Buffer* m_instanceDataBuffers[MAX_FRAMES_IN_FLIGHT];
    
    /// Camera data buffer
    MTL::Buffer* m_cameraDataBuffer;
    
    /// Buffer for the fragment shader to store the textures inside of
    MTL::Buffer* m_fragmentShaderArgBuffer;
    
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
    
    simd::float3 m_playerPos = (simd::float3) { 0.f, 0.f, -20.f };
    
    int recalculateBlocks = MAX_FRAMES_IN_FLIGHT;
    
    // Basic block array that will mimick an actual implementation we'll have down the line
    // 1 means a block is there
    // 0 means a block is not there
    uint64_t blocks[16 * 16 * 16] =
    {
        1
    };
    
    int instanceCount = 0;
};

#endif /* Renderer_hpp */
