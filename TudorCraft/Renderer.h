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

class Renderer {
public:
    Renderer( MTL::Device* pDevice );
    ~Renderer();
    
    void loadMetal();
    
    MTL::Texture *loadTextureUsingAtlas();
    
    void windowSizeWillChange(unsigned int width, unsigned int height);
    
    void draw(MTL::RenderPassDescriptor *currentRPD, MTL::Drawable* currentDrawable);

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
    MTL::Buffer* m_instanceDataBuffer;
    
    /// Camera data buffer
    MTL::Buffer* m_cameraDataBuffer;
    
    /// Number of vertices
    NS::UInteger m_verticesCount;
    
    /// Window size
    vector_uint2 m_windowSize;
    
    /// The atlas we're using to load the textures from.
    TextureAtlas *m_atlas;
    
    float m_angle = 0.0;
};

#endif /* Renderer_hpp */
