//
//  Renderer.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 02.11.2022.
//

// MARK: TODO
/*
 - Fix the memory leak!!!
 - Clean up the code.
 - Create folders for the renderer, for the shader stuff, rest of the app, etc.
 - Do more than a triangle. :)
 - Use the bragging rights to their full potential.
 - Fully grasp what I wrote here. :)
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/sysctl.h>
#include <simd/simd.h>

#define NS_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>
#define MTL_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>
#define CA_PRIVATE_IMPLEMENTATION
#include <QuartzCore/QuartzCore.hpp>

#include "AAPLUtilities.h"

#include "Renderer.h"
#include "ShaderTypes.h"
#include "Math3D.hpp"

Renderer::Renderer( MTL::Device* pDevice )
: m_device( pDevice->retain() ) {
    m_commandQueue = m_device->newCommandQueue();
    
    AAPL_PRINT("Metal device used to render: ", m_device->name()->utf8String());
    
    m_semaphore = dispatch_semaphore_create(MAX_FRAMES_IN_FLIGHT);
    
    // Create a blank texture atlas
    m_atlas = new TextureAtlas();
    
    m_frame = 0;
    
    loadMetal();
}

Renderer::~Renderer() {
    m_pipelineState->release();
    m_commandQueue->release();
    m_device->release();
    m_vertices->release();
    m_indexBuffer->release();
    
    for ( int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ ) {
        m_instanceDataBuffers[i]->release();
    }
    
    m_depthState->release();
    m_texture->release();
    
    delete m_atlas;
}

MTL::Texture *Renderer::loadTextureUsingAtlas() {
    // Get image from storage and load it in the atlas
    m_atlas->loadAtlasInMemory();
    
    // Indicate that each pixel has a blue, green, red, and alpha channel, where each channel is
    MTL::TextureDescriptor *textureDescriptor = MTL::TextureDescriptor::alloc()->init();
    textureDescriptor->setPixelFormat(MTL::PixelFormatBGRA8Unorm_sRGB);
    
    // Set the pixel dimensions of the texture
    textureDescriptor->setWidth(m_atlas->getWidth());
    textureDescriptor->setHeight(m_atlas->getHeight());
    
    // Create the texture from the device by using the descriptor
    MTL::Texture *texture = m_device->newTexture(textureDescriptor);
    
    NS::UInteger bytesPerRow = m_atlas->getChannels() * m_atlas->getWidth();
    
    MTL::Region region = MTL::Region(0, 0, 0, // MTLOrigin
                                     m_atlas->getWidth(), m_atlas->getHeight(), 1); // MTLSize
    
    // Copy the bytes from the data object into the texture
    texture->replaceRegion(region, 0, m_atlas->getRawData(), bytesPerRow);
    return texture;
}

void Renderer::loadMetal() {
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();
    
    // MARK: Create texture
    m_texture = loadTextureUsingAtlas();
    
    float s = 0.5f;
    
    // MARK: Create vertices
    Vertex quadVertices[] =
    {
        //                                         Texture
        //   Positions           Normals         Coordinates
        { { -s, -s, +s }, {  0.f,  0.f,  1.f }, { 0.f, 1.f } },
        { { +s, -s, +s }, {  0.f,  0.f,  1.f }, { 1.f, 1.f } },
        { { +s, +s, +s }, {  0.f,  0.f,  1.f }, { 1.f, 0.f } },
        { { -s, +s, +s }, {  0.f,  0.f,  1.f }, { 0.f, 0.f } },

        { { +s, -s, +s }, {  1.f,  0.f,  0.f }, { 0.f, 1.f } },
        { { +s, -s, -s }, {  1.f,  0.f,  0.f }, { 1.f, 1.f } },
        { { +s, +s, -s }, {  1.f,  0.f,  0.f }, { 1.f, 0.f } },
        { { +s, +s, +s }, {  1.f,  0.f,  0.f }, { 0.f, 0.f } },

        { { +s, -s, -s }, {  0.f,  0.f, -1.f }, { 0.f, 1.f } },
        { { -s, -s, -s }, {  0.f,  0.f, -1.f }, { 1.f, 1.f } },
        { { -s, +s, -s }, {  0.f,  0.f, -1.f }, { 1.f, 0.f } },
        { { +s, +s, -s }, {  0.f,  0.f, -1.f }, { 0.f, 0.f } },

        { { -s, -s, -s }, { -1.f,  0.f,  0.f }, { 0.f, 1.f } },
        { { -s, -s, +s }, { -1.f,  0.f,  0.f }, { 1.f, 1.f } },
        { { -s, +s, +s }, { -1.f,  0.f,  0.f }, { 1.f, 0.f } },
        { { -s, +s, -s }, { -1.f,  0.f,  0.f }, { 0.f, 0.f } },

        { { -s, +s, +s }, {  0.f,  1.f,  0.f }, { 0.f, 1.f } },
        { { +s, +s, +s }, {  0.f,  1.f,  0.f }, { 1.f, 1.f } },
        { { +s, +s, -s }, {  0.f,  1.f,  0.f }, { 1.f, 0.f } },
        { { -s, +s, -s }, {  0.f,  1.f,  0.f }, { 0.f, 0.f } },

        { { -s, -s, -s }, {  0.f, -1.f,  0.f }, { 0.f, 1.f } },
        { { +s, -s, -s }, {  0.f, -1.f,  0.f }, { 1.f, 1.f } },
        { { +s, -s, +s }, {  0.f, -1.f,  0.f }, { 1.f, 0.f } },
        { { -s, -s, +s }, {  0.f, -1.f,  0.f }, { 0.f, 0.f } }
    };

    uint16_t indices[] = {
         0,  1,  2,  2,  3,  0, /* front */
         4,  5,  6,  6,  7,  4, /* right */
         8,  9, 10, 10, 11,  8, /* back */
        12, 13, 14, 14, 15, 12, /* left */
        16, 17, 18, 18, 19, 16, /* top */
        20, 21, 22, 22, 23, 20, /* bottom */
    };
    
    // MARK: Create buffers
    m_vertices = m_device->newBuffer(quadVertices, sizeof(quadVertices), MTL::ResourceStorageModeShared);
    m_indexBuffer = m_device->newBuffer(indices, sizeof(indices), MTL::ResourceStorageModeShared);
    m_cameraDataBuffer = m_device->newBuffer(sizeof(CameraData), MTL::ResourceStorageModeShared);
    
    // Create a buffer for each frame we can work independently
    for ( int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ ) {
        m_instanceDataBuffers[i] = m_device->newBuffer(2 * sizeof(InstanceData), MTL::ResourceStorageModeShared);
    }
    
    //MARK: Create render pipeline
    
    // Load all the shader files with the .metal extensions in the project
    MTL::Library *defaultLibrary = m_device->newDefaultLibrary();
    
    MTL::Function *vertexFunction = defaultLibrary->newFunction(AAPLSTR("vertexShader"));
    MTL::Function *fragmentFunction = defaultLibrary->newFunction(AAPLSTR("samplingShader"));
    
    AAPL_ASSERT(vertexFunction, "Couldn't create vertexFunction");
    AAPL_ASSERT(fragmentFunction, "Couldn't create fragmentFunction");
    
    // Configure a pipeline descrmiptor that is used to create a pipeline state.
    MTL::RenderPipelineDescriptor *pipeLineStateDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    pipeLineStateDescriptor->setLabel(AAPLSTR("MySuperAwesomePipeline!"));
    pipeLineStateDescriptor->setVertexFunction(vertexFunction);
    pipeLineStateDescriptor->setFragmentFunction(fragmentFunction);
    pipeLineStateDescriptor->colorAttachments()->object(NS::UInteger(0))->setPixelFormat(MTL::PixelFormatBGRA8Unorm_sRGB);
    pipeLineStateDescriptor->setDepthAttachmentPixelFormat(MTL::PixelFormatDepth32Float);
    
    NS::Error *err = nullptr;
    m_pipelineState = m_device->newRenderPipelineState(pipeLineStateDescriptor, &err);
    
    AAPL_ASSERT_NULL_ERROR(err, "Failed to create pipeline state...");
    
    //MARK: - Create depth/stenctil object
    MTL::DepthStencilDescriptor* depthDescriptor = MTL::DepthStencilDescriptor::alloc()->init();
    depthDescriptor->setDepthCompareFunction(MTL::CompareFunctionLessEqual);
    depthDescriptor->setDepthWriteEnabled(true);
    m_depthState = m_device->newDepthStencilState(depthDescriptor);
    
    depthDescriptor->release();
    pipeLineStateDescriptor->release();
    vertexFunction->release();
    fragmentFunction->release();
    defaultLibrary->release();
    
    pPool->release();
}

void Renderer::windowSizeWillChange(unsigned int width, unsigned int height) {
    m_windowSize.x = width;
    m_windowSize.y = height;
};

void Renderer::draw(MTL::RenderPassDescriptor *currentRPD, MTL::Drawable* currentDrawable) {
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();
    
    using namespace simd;
    
    const float scl = 10.f;
    
    m_frame = (m_frame + 1) % MAX_FRAMES_IN_FLIGHT;
    MTL::Buffer* instanceDataBuffer = m_instanceDataBuffers[m_frame];
    
    // Create a new command buffer for each render pass to the current drawable.
    MTL::CommandBuffer *commandBuffer = m_commandQueue->commandBuffer();
    commandBuffer->setLabel(AAPLSTR("MyAwesomeCommand"));
    
    // Wait for the instance buffer to be available
    dispatch_semaphore_wait( m_semaphore, DISPATCH_TIME_FOREVER );
    Renderer* pRenderer = this;
    commandBuffer->addCompletedHandler( ^void( MTL::CommandBuffer* pCmd ) {
        dispatch_semaphore_signal( pRenderer->m_semaphore );
    });
    
    // Apply changes to the instance
    InstanceData *instanceData = reinterpret_cast<InstanceData *>(instanceDataBuffer->contents());
    
    float3 object1Position = { 0.f, 0.f, 0.f };
    float3 object2Position = { 10.f, 0.f, 0.f };
    
    float4x4 scale = Math3D::makeScale( (float3){ scl, scl, scl } );
    float4x4 rt1 = Math3D::makeTranslate( object1Position );
    float4x4 rt2 = Math3D::makeTranslate( object2Position );
    
    float4x4 fullObject1Rot = rt1 * scale;
    float4x4 fullObject2Rot = rt2 *  scale;
    
    instanceData[0].transform = fullObject1Rot;
    instanceData[0].normalTransform = Math3D::discardTranslation(instanceData[0].transform);
    
    instanceData[1].transform = fullObject2Rot;
    instanceData[1].normalTransform = Math3D::discardTranslation(instanceData[1].transform);
    
    // Apply rotations to the world and the camera
    CameraData *cameraData = reinterpret_cast<CameraData *>(m_cameraDataBuffer->contents());
    cameraData->perspectiveTranform = Math3D::makePerspective(45.f * M_PI / 180.f, 1.f, 0.03f, 500.0f);
    cameraData->worldTranform = Math3D::makeTranslate( (float3){0.f, 0.f, -50.f} );
    cameraData->worldNormalTranform = Math3D::discardTranslation(cameraData->worldTranform);
    
    if ( currentRPD != nullptr ) {
        // Create a render command encoder.
        MTL::RenderCommandEncoder *renderEncoder = commandBuffer->renderCommandEncoder(currentRPD);
        
        // Set the region of the drawable to draw into.
        MTL::Viewport viewPort = {0.0, 1.0, (double) m_windowSize.x, (double) m_windowSize.y, 0.0, 1.0};
        renderEncoder->setViewport(viewPort);
        
        renderEncoder->setRenderPipelineState(m_pipelineState);
        
        renderEncoder->setDepthStencilState(m_depthState);
        
        // Pass in the parameter data.
        renderEncoder->setVertexBuffer(m_vertices, 0, VertexInputIndexVertices);
        
        renderEncoder->setVertexBuffer(instanceDataBuffer, 0, VertexInputIndexInstanceData);
        
        renderEncoder->setVertexBuffer(m_cameraDataBuffer, 0, VertexInputIndexCameraData);
         
        renderEncoder->setFragmentTexture(m_texture, TextureIndexBaseColor);
        
        // Set modes for 3D rendering
        renderEncoder->setCullMode( MTL::CullModeBack );
        renderEncoder->setFrontFacingWinding( MTL::Winding::WindingCounterClockwise );
        
        // Draw the triangles.
        renderEncoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, // Primitive type
                                             36, // Index count
                                             MTL::IndexType::IndexTypeUInt16, // Index type
                                             m_indexBuffer, // Index buffer
                                             0, // Index buffer offset
                                             2); // Instance count
        
        renderEncoder->endEncoding();
        
        commandBuffer->presentDrawable(currentDrawable);
    }
    
    commandBuffer->commit();
    
    pPool->release();
}
