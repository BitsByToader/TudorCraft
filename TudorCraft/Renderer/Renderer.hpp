//
//  Renderer.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 02.11.2022.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include <mutex>

#include <Metal/Metal.hpp>
#include <simd/simd.h>
#include "ShaderTypes.h"
#include "TextureAtlas.hpp"

class Renderer {
public:
    // MARK: Public Methods
    /// Generic constructor for default Renderer (which is what will be used)
    Renderer();
    
    /// Default destructor.
    ~Renderer();
    
    /// Getter for the global Renderer singleton.
    static Renderer *shared();
    
    /// Notifies the renderer that the window size changed.
    /// - Parameters:
    ///   - width: The new width of the window.
    ///   - height: The new height of the window.
    void windowSizeWillChange(unsigned int width, unsigned int height);
    
    
    /// Tells the renderer to draw in the drawable using the render descriptor.
    /// - Parameters:
    ///   - currentRPD: Render pass descriptor to use.
    ///   - currentDrawable: Drawable to draw in.
    void draw(MTL::RenderPassDescriptor *currentRPD, MTL::Drawable* currentDrawable);
    
    
    /// Getter for `m_instanceDataBuffer`.
    InstanceData* instanceBuffer();
    
    /// Getter for a pointer to`m_instanceCount`.
    int *instanceCount();
    
    /// Removes the `InstanceData` from `m_instanceDataBuffer` at the specifiec index.
    ///
    /// Removing is done by taking the last element from `m_instanceDataBuffer` and placing at the index that was removed.
    /// This method automatically decreases `m_instanceCount`.
    /// - Parameter index: The element's index to remove from the buffer.
    void removeInstanceAt(int index);
    
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

    std::mutex m_gpuMutex;
    
    // Change me
    simd::float3 m_playerPos = (simd::float3) { 0.f, 0.f, 0.f };
    simd::float2 m_centerChunk = (simd::float2) { 0.f, 0.f };
    
private:
    static Renderer *globalObject;
    
    // MARK: Private methods
    void loadMetal();
    void createHeap();
    void moveResourcesToHeap();
    static MTL::TextureDescriptor *newDescriptorFromTexture(MTL::Texture *texture,
                                                            MTL::StorageMode storageMode);
    
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
    MTL::Buffer* m_instanceDataBuffer;
    
    /// Number of instances in any of the data buffers
    int m_instanceCount = 0;
    
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
    
    /// Frame we're currently working on
    int m_frame;
    
    float m_yawAngle = 0.f;
    float m_pitchAngle = 0.f;
};

#endif /* Renderer_hpp */
