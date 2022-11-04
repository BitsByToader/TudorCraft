//
//  Renderer.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 02.11.2022.
//

// MARK: TODO
/*
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

Renderer::Renderer( MTL::Device* pDevice )
: m_device( pDevice->retain() ) {
    m_commandQueue = m_device->newCommandQueue();
    
    AAPL_PRINT("Metal device used to render: ", m_device->name()->utf8String());
    
    loadMetal();
}

Renderer::~Renderer() {
    m_pipelineState->release();
    m_commandQueue->release();
    m_device->release();
}

void Renderer::loadMetal() {
    // Load all the shader files with the .metal extensions in the project
    MTL::Library *defaultLibrary = m_device->newDefaultLibrary();
    
    MTL::Function *vertexFunction = defaultLibrary->newFunction(AAPLSTR("vertexShader"));
    MTL::Function *fragmentFunction = defaultLibrary->newFunction(AAPLSTR("fragmentShader"));
    
    AAPL_ASSERT(vertexFunction, "Couldn't create vertexFunction");
    AAPL_ASSERT(fragmentFunction, "Couldn't create fragmentFunction");
    
    // Configure a pipeline descrmiptor that is used to create a pipeline state.
    MTL::RenderPipelineDescriptor *pipeLineStateDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    pipeLineStateDescriptor->setLabel(AAPLSTR("A pipeline?"));
    pipeLineStateDescriptor->setVertexFunction(vertexFunction);
    pipeLineStateDescriptor->setFragmentFunction(fragmentFunction);
    pipeLineStateDescriptor->colorAttachments()->object(NS::UInteger(0))->setPixelFormat(MTL::PixelFormatBGRA8Unorm_sRGB);
    
    NS::Error *err = nullptr;
    m_pipelineState = m_device->newRenderPipelineState(pipeLineStateDescriptor, &err);
    
    AAPL_ASSERT_NULL_ERROR(err, "Failed to create pipeline state...");
    
    pipeLineStateDescriptor->release();
    vertexFunction->release();
    fragmentFunction->release();
    defaultLibrary->release();
}

void Renderer::windowSizeWillChange(unsigned int width, unsigned int height) {
    m_windowSize.x = width;
    m_windowSize.y = height;
};

void Renderer::draw(MTL::RenderPassDescriptor *currentRPD, MTL::Drawable* currentDrawable) {
    static const Vertex triangleVertices[] =
    {
        // 2D Positions,   RGBA colours
        { {  250,  -250 }, { 1, 0, 0, 1 } },
        { { -250,  -250 }, { 0, 1, 0, 1 } },
        { {    0,   250 }, { 0, 0, 1, 1 } },
    };
    
    // Create a new command buffer for each render pass to the current drawable.
    MTL::CommandBuffer *commandBuffer = m_commandQueue->commandBuffer();
    commandBuffer->setLabel(AAPLSTR("Simple Command"));
    
    if ( currentRPD != nullptr ) {
        // Create a render command encoder.
        MTL::RenderCommandEncoder *renderEncoder = commandBuffer->renderCommandEncoder(currentRPD);
        
        // Set the region of the drawable to draw into.
        MTL::Viewport viewPort = {0.0, 1.0, (double) m_windowSize.x, (double) m_windowSize.y, 0.0, 1.0};
        renderEncoder->setViewport(viewPort);
        
        renderEncoder->setRenderPipelineState(m_pipelineState);
        
        // Pass in the parameter data.
        renderEncoder->setVertexBytes(triangleVertices,
                                      sizeof(triangleVertices),
                                      VertexInputIndexVertices);
        
        renderEncoder->setVertexBytes(&m_windowSize,
                                      sizeof(m_windowSize),
                                      VertexInputIndexViewportSize);
        
        // Draw the triangle.
        renderEncoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));
        
        renderEncoder->endEncoding();
        
        commandBuffer->presentDrawable(currentDrawable);
    }
    
    commandBuffer->commit();
}
