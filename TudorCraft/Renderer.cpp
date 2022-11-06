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
    
    // Create a blank texture atlas
    m_atlas = new TextureAtlas();
    
    loadMetal();
}

Renderer::~Renderer() {
    m_pipelineState->release();
    m_commandQueue->release();
    m_device->release();
    
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
    
    // MARK: Create vertices
    static Vertex quadVertices[] =
    {
        // Pixel positions+depth, Texture coordinates
        
        //Triangle Left
        { {  250,  -250, 1.0 },  { 1.f, 1.f } },
        { { -250,  -250, 1.0 },  { 0.f, 1.f } },
        { { -250,   250, 1.0 },  { 0.f, 0.f } },

        //Triangle Right
        { {  250,  -250, 1.0 },  { 1.f, 1.f } },
        { { -250,   250, 1.0 },  { 0.f, 0.f } },
        { {  250,   250, 1.0 },  { 1.f, 0.f } },
        
        // => a magnificient quad
    };
    
    // Create a vertex buffer, and initialize it with the quadVertices array
    m_vertices = m_device->newBuffer(quadVertices, sizeof(quadVertices), MTL::ResourceStorageModeShared);
    
    m_verticesCount = sizeof(quadVertices) / sizeof(Vertex);
    
    //MARK: Create render pipeline
    
    // Load all the shader files with the .metal extensions in the project
    MTL::Library *defaultLibrary = m_device->newDefaultLibrary();
    
    MTL::Function *vertexFunction = defaultLibrary->newFunction(AAPLSTR("vertexShader"));
    MTL::Function *fragmentFunction = defaultLibrary->newFunction(AAPLSTR("samplingShader"));
    
    AAPL_ASSERT(vertexFunction, "Couldn't create vertexFunction");
    AAPL_ASSERT(fragmentFunction, "Couldn't create fragmentFunction");
    
    // Configure a pipeline descrmiptor that is used to create a pipeline state.
    MTL::RenderPipelineDescriptor *pipeLineStateDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    pipeLineStateDescriptor->setLabel(AAPLSTR("My super awesome pipeline!"));
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
    
    Vertex *quad  = reinterpret_cast<Vertex *>(m_vertices->contents());
    
    for ( int i = 0; i < m_verticesCount; i++ ) {
        if ( _right )
            quad[i].position.x += 10;
        else
            quad[i].position.x -= 10;
    }
    
    if ( quad[0].position.x >= 500 ) {
        _right = false;
    }
    if ( quad[1].position.x <= -500 ) {
        _right = true;
    }
    
    // didModifyRange is only necessary for managed resource mode as the CPU and GPU both hold a copy of the data and syncing is necessary
    // However, this mode is not supported on Apple GPUs since they used an unified memory model.
    // Shared memory model is compatible on all GPUs and also doesn't need syncing, so remove the line bellow.
    //    m_vertices->didModifyRange(NS::Range::Make(0, m_verticesCount));
    
    // Create a new command buffer for each render pass to the current drawable.
    MTL::CommandBuffer *commandBuffer = m_commandQueue->commandBuffer();
    commandBuffer->setLabel(AAPLSTR("Texturing Command"));
    
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
        
        renderEncoder->setVertexBytes(&m_windowSize,
                                      sizeof(m_windowSize),
                                      VertexInputIndexViewportSize);
         
        // Set the texture object.  The AAPLTextureIndexBaseColor enum value corresponds
        //  to the 'colorMap' argument in the 'samplingShader' function because its
        //   texture attribute qualifier also uses AAPLTextureIndexBaseColor for its index.
        renderEncoder->setFragmentTexture(m_texture, TextureIndexBaseColor);
        
        // Draw the triangle.
        renderEncoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0), m_verticesCount);
        renderEncoder->endEncoding();
        
        commandBuffer->presentDrawable(currentDrawable);
    }
    
    commandBuffer->commit();
    
    pPool->release();
}
