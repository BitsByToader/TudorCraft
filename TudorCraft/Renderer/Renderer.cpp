//
//  Renderer.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 02.11.2022.
//

// MARK: TODO
/*
 - Clean up the code.
 - Use the bragging rights to their full potential :-)
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

// MARK: - Constructor
Renderer::Renderer() {
    m_device = MTL::CreateSystemDefaultDevice();
    m_device->retain();
    
    m_commandQueue = m_device->newCommandQueue();
    
    AAPL_PRINT("Metal device used to render:", m_device->name()->utf8String());
    
    m_semaphore = dispatch_semaphore_create(MAX_FRAMES_IN_FLIGHT);
    
    // Create a blank texture atlas
    m_atlas = new TextureAtlas(m_device);
    m_atlas->loadAtlasInMemory();
    
    m_frame = 0;
    
    loadMetal();
}

// MARK: - Destructor
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
    
    for ( int i = 0; i < 3; i++ ) {
        m_texture[i]->release();
    }
}

// MARK: - Transformation matrixes for mesh creation
const inline simd::float4x4 moveFaceToBack() {
    return Math3D::makeYRotate4x4(M_PI);
}

const inline simd::float4x4 moveFaceToRight() {
    return Math3D::makeYRotate4x4(M_PI/2);
}

const inline simd::float4x4 moveFaceToLeft() {
    return Math3D::makeYRotate4x4(3 * M_PI/2);
}

const inline simd::float4x4 moveFaceToTop() {
    return Math3D::makeXRotate4x4(M_PI/2);
}

const inline simd::float4x4 moveFaceToBottom() {
    return Math3D::makeXRotate4x4(3 * M_PI/2);
}

//MARK: - Load metal
void Renderer::loadMetal() {
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();
    
    // MARK: Create vertices
    float s = 0.5f;
    Vertex quadVertices[] =
    {
        //                                         Texture
        //   Positions           Normals         Coordinates
        { { -s, -s, +s }, {  0.f,  0.f,  1.f }, { 0.f, 1.f } }, // 0
        { { +s, -s, +s }, {  0.f,  0.f,  1.f }, { 1.f, 1.f } }, // 1
        { { +s, +s, +s }, {  0.f,  0.f,  1.f }, { 1.f, 0.f } }, // 2
        { { -s, +s, +s }, {  0.f,  0.f,  1.f }, { 0.f, 0.f } }, // 3
    };

    uint16_t indices[] = {
         0,  1,  2,  2,  3,  0, /* front */
    };
    
    // MARK: Create buffers
    m_vertices = m_device->newBuffer(quadVertices, sizeof(quadVertices), MTL::ResourceStorageModeShared);
    m_indexBuffer = m_device->newBuffer(indices, sizeof(indices), MTL::ResourceStorageModeShared);
    
    // Create the buffer and the perspective matrix once
    m_cameraDataBuffer = m_device->newBuffer(sizeof(CameraData), MTL::ResourceStorageModeShared);
    
    // Create a buffer for each frame we can work independently
    for ( int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ ) {
        // number of blocks times the number of faces we can possibly have
        m_instanceDataBuffers[i] = m_device->newBuffer(16 * 16 * 16 * 6 * sizeof(InstanceData), MTL::ResourceStorageModeShared);
    }
    
    for ( int i = 0; i < 16*16*16; i++ ) {
        blocks[i] = 1;
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
    
    //MARK: Load the textures into a buffer array
    MTL::ArgumentEncoder *argEncoder = fragmentFunction->newArgumentEncoder(FragmentInputIndexTextures);
    
    NS::UInteger argBuffLength = argEncoder->encodedLength();
    m_fragmentShaderArgBuffer = m_device->newBuffer(argBuffLength, MTL::ResourceStorageModeShared);
    m_fragmentShaderArgBuffer->setLabel(AAPLSTR("Argument Buffer Fragment Shader"));
    
    argEncoder->setArgumentBuffer(m_fragmentShaderArgBuffer, 0);
    
    m_texture[0] = m_atlas->getTextureWithCoordinates(0, 25*16-9); // side
    m_texture[1] = m_atlas->getTextureWithCoordinates(17*16-6, 0); // top
    m_texture[2] = m_atlas->getTextureWithCoordinates(10*16-4, 21*16-8); // bottom
    
    for ( int i = 0; i < 3; i++ ) {
        argEncoder->setTexture(m_texture[i], TextureIndexBaseColor + i);
    }
    
    //MARK: Create depth/stenctil object
    MTL::DepthStencilDescriptor* depthDescriptor = MTL::DepthStencilDescriptor::alloc()->init();
    depthDescriptor->setDepthCompareFunction(MTL::CompareFunctionLessEqual);
    depthDescriptor->setDepthWriteEnabled(true);
    m_depthState = m_device->newDepthStencilState(depthDescriptor);
    
    //MARK: Free memory
    argEncoder->release();
    depthDescriptor->release();
    pipeLineStateDescriptor->release();
    vertexFunction->release();
    fragmentFunction->release();
    defaultLibrary->release();
    
    // After loading all the textures in the GPU Heap, we don't need the atlas anymore, so delete it.
    delete m_atlas;
    m_atlas = nullptr;
    
    pPool->release();
}

//MARK: - Calculate mesh faces
int Renderer::calculateMeshes(InstanceData *instanceData) {
    using namespace simd;
    const float scl = 5.f;
    
    int instanceCount = 0;
    
    int limit_x = 16;
    int limit_y = 16;
    int limit_z = 16;

    const float4x4 scale = Math3D::makeScale( (float3){ scl, scl, scl } );

    for ( int k = 0; k < limit_y; k++ ) {
        for ( int i = 0; i < limit_z; i++ ) {
            for ( int j = 0; j < limit_x; j++ ) {
                if ( blocks[k * limit_x * limit_z + i*limit_x + j] != 0 ) {
                    // Loop through every single block
                    // Create the main block position
                    float4x4 blockPositionTranslationMatrix = Math3D::makeTranslate((float3) { 5.f * j, 5.f * k, -(5.f * i) } );

                    if ( i == 0 || blocks[(k * limit_x * limit_z) + ((i-1) * limit_x) + j] == 0 ) {
                        // Front of block
                        instanceData[instanceCount].transform = blockPositionTranslationMatrix * scale;
                        instanceData[instanceCount].normalTransform = Math3D::discardTranslation(instanceData[instanceCount].transform);
                        instanceData[instanceCount].textureId = 0;
                        
                        instanceCount++;
                    }

                    if ( j == 0 || blocks[(k * limit_x * limit_z) + (i * limit_x) + (j - 1)] == 0 ) {
                        // Left of block
                        instanceData[instanceCount].transform = blockPositionTranslationMatrix * scale * moveFaceToLeft();
                        instanceData[instanceCount].normalTransform = Math3D::discardTranslation(instanceData[instanceCount].transform);
                        instanceData[instanceCount].textureId = 0;
                        
                        instanceCount++;
                    }

                    if ( i == limit_z - 1 || blocks[(k * limit_x * limit_z) + ((i+1) * limit_x) + j] == 0 ){
                        // Back of block
                        instanceData[instanceCount].transform = blockPositionTranslationMatrix * scale * moveFaceToBack();
                        instanceData[instanceCount].normalTransform = Math3D::discardTranslation(instanceData[instanceCount].transform);
                        instanceData[instanceCount].textureId = 0;
                        
                        instanceCount++;
                    }

                    if ( j == limit_x-1 || blocks[(k * limit_x * limit_z) + (i * limit_x) + (j + 1)] == 0 ){
                        // Right of block
                        instanceData[instanceCount].transform = blockPositionTranslationMatrix * scale * moveFaceToRight();
                        instanceData[instanceCount].normalTransform = Math3D::discardTranslation(instanceData[instanceCount].transform);
                        instanceData[instanceCount].textureId = 0;
                        
                        instanceCount++;
                    }
                    
                    if ( k == limit_y - 1 || blocks[((k+1) * limit_x * limit_z) + (i * limit_x) + j] == 0 ){
                        // Top of block
                        instanceData[instanceCount].transform = blockPositionTranslationMatrix * scale * moveFaceToTop();
                        instanceData[instanceCount].normalTransform = Math3D::discardTranslation(instanceData[instanceCount].transform);
                        instanceData[instanceCount].textureId = 1;
                        
                        instanceCount++;
                    }

                    if ( k == 0 || blocks[((k-1) * limit_x * limit_z) + (i * limit_x) + j] == 0 ) {
                        // Bottom of block
                        instanceData[instanceCount].transform = blockPositionTranslationMatrix * scale * moveFaceToBottom();
                        instanceData[instanceCount].normalTransform = Math3D::discardTranslation(instanceData[instanceCount].transform);
                        instanceData[instanceCount].textureId = 2;
                        
                        instanceCount++;
                    }
                }
            }
        }
    }
    
    return instanceCount;
};

// MARK: - Window size will change method
void Renderer::windowSizeWillChange(unsigned int width, unsigned int height) {
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();
    
    m_windowSize.x = width;
    m_windowSize.y = height;
    CameraData *cameraData = reinterpret_cast<CameraData *>(m_cameraDataBuffer->contents());
    cameraData->perspectiveTranform = Math3D::makePerspective(90.f * M_PI / 180.f, m_windowSize.x / m_windowSize.y, 0.03f, 500.0f);
    
    pPool->release();
};

// MARK: - Draw method
void Renderer::draw(MTL::RenderPassDescriptor *currentRPD, MTL::Drawable* currentDrawable) {
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();
    
    using namespace simd;
    
    m_frame = (m_frame + 1) % MAX_FRAMES_IN_FLIGHT;
    MTL::Buffer* instanceDataBuffer = m_instanceDataBuffers[m_frame];
    
    // Create a new command buffer for each render pass to the current drawable.
    MTL::CommandBuffer *commandBuffer = m_commandQueue->commandBuffer();
    commandBuffer->setLabel(AAPLSTR("WorldDrawingCommand"));
    
    // Wait for the instance buffer to be available
    dispatch_semaphore_wait( m_semaphore, DISPATCH_TIME_FOREVER );
    Renderer* pRenderer = this;
    commandBuffer->addCompletedHandler( ^void( MTL::CommandBuffer* pCmd ) {
        dispatch_semaphore_signal( pRenderer->m_semaphore );
    });
    
    if ( recalculateBlocks ) {
        InstanceData *instanceData = reinterpret_cast<InstanceData *>(instanceDataBuffer->contents());
        
        instanceCount = calculateMeshes(instanceData);
        
        --recalculateBlocks;
    }
    
    // MARK: World transformations
    CameraData *cameraData = reinterpret_cast<CameraData *>(m_cameraDataBuffer->contents());
    cameraData->worldTranform = Math3D::makeXRotate4x4(m_pitchAngle) * Math3D::makeYRotate4x4(m_yawAngle) * Math3D::makeTranslate( m_playerPos );
    cameraData->worldNormalTranform = Math3D::discardTranslation(cameraData->worldTranform);
    
    // MARK: Configure draw command
    if ( currentRPD != nullptr ) {
        // Create a render command encoder.
        MTL::RenderCommandEncoder *renderEncoder = commandBuffer->renderCommandEncoder(currentRPD);
        
        // Set the region of the drawable to draw into.
        MTL::Viewport viewPort = {0.0, 0.0, (double) m_windowSize.x, (double) m_windowSize.y, 0.0, 1.0};
        renderEncoder->setViewport(viewPort);
        
        // Indicate to Metal that the textures will be mapped to the GPUs address space
        for ( int i = 0; i < 3; i++ ) {
            renderEncoder->useResource(m_texture[i], MTL::ResourceUsageSample);
        }
        
        renderEncoder->setRenderPipelineState(m_pipelineState);
        
        renderEncoder->setDepthStencilState(m_depthState);
        
        // Pass in the parameter data.
        renderEncoder->setVertexBuffer(m_vertices, 0, VertexInputIndexVertices);
        renderEncoder->setVertexBuffer(instanceDataBuffer, 0, VertexInputIndexInstanceData);
        renderEncoder->setVertexBuffer(m_cameraDataBuffer, 0, VertexInputIndexCameraData);
        // TODO: This might need some optimization?
        renderEncoder->setFragmentBuffer(m_fragmentShaderArgBuffer, 0, FragmentInputIndexTextures);
        renderEncoder->setFragmentBuffer(m_cameraDataBuffer, 0, FragmentInputIndexCameraData);
        
        // Set modes for 3D rendering
        renderEncoder->setCullMode( MTL::CullModeBack );
        renderEncoder->setFrontFacingWinding( MTL::Winding::WindingCounterClockwise );
        
        // Draw our cubes.
        renderEncoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, // Primitive type
                                             6, // Index count
                                             MTL::IndexType::IndexTypeUInt16, // Index type
                                             m_indexBuffer, // Index buffer
                                             0, // Index buffer offset
                                             instanceCount); // Instance count
        
        renderEncoder->endEncoding();
        commandBuffer->presentDrawable(currentDrawable);
    }
    
    commandBuffer->commit();
    
    pPool->release();
}

// MARK: - Player moving methods
void Renderer::forward() {
    m_playerPos.x += sinf(-m_yawAngle);
    
    m_playerPos.z += cosf(m_yawAngle) * cosf(m_pitchAngle);
    m_playerPos.y += cosf(m_yawAngle) * sinf(-m_pitchAngle);
};
void Renderer::backward() {
    m_playerPos.x -= sinf(-m_yawAngle);
    
    m_playerPos.z -= cosf(m_yawAngle) * cosf(m_pitchAngle);
    m_playerPos.y -= cosf(m_yawAngle) * sinf(-m_pitchAngle);
};
void Renderer::left() {
    m_playerPos.x -= -cosf(m_yawAngle);
    m_playerPos.z -= sinf(-m_yawAngle);
};
void Renderer::right() {
    m_playerPos.x += -cosf(m_yawAngle);
    m_playerPos.z += sinf(-m_yawAngle);
};

void Renderer::up() {
    m_playerPos.y -= 1.f;
};

void Renderer::down() {
    m_playerPos.y += 1.f;
};

void Renderer::lookUp() {
    if ( m_pitchAngle < M_PI/2) {
        m_pitchAngle += 0.025f;
    }
};

void Renderer::lookDown() {
    if ( m_pitchAngle > -M_PI/2 ) {
        m_pitchAngle -= 0.025f;
    }
};

void Renderer::lookRight() {
    if ( m_yawAngle == 2*M_PI ) {
        m_yawAngle = 0;
    } else {
        m_yawAngle += 0.025f;
    }
};

void Renderer::lookLeft() {
    if (m_yawAngle == -2*M_PI ) {
        m_yawAngle = 0;
    } else {
        m_yawAngle -= 0.025f;
    }
};
