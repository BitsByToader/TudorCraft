
//  Renderer.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 02.11.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/sysctl.h>
#include <simd/simd.h>
#include <chrono>
#include <ctime>

#define NS_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>
#define MTL_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>
#define CA_PRIVATE_IMPLEMENTATION
#include <QuartzCore/QuartzCore.hpp>

#include "Renderer.hpp"
#include "Chunk.hpp"
#include "EntityComponent.hpp"
#include "AAPLUtilities.h"
#include "ShaderTypes.h"
#include "Math3D.hpp"

//MARK: - Singleton
Renderer *Renderer::m_sharedObject = nullptr;
Renderer *Renderer::shared() {
    if ( m_sharedObject == nullptr ) {
        m_sharedObject = new Renderer();
    };
    
    return m_sharedObject;
}

void Renderer::destroySharedObject() {
    delete m_sharedObject;
    m_sharedObject = nullptr;
}

// MARK: - Constructor
Renderer::Renderer() {
    m_device = MTL::CreateSystemDefaultDevice();
    m_device->retain();
    
    m_commandQueue = m_device->newCommandQueue();
    
    AAPL_PRINT("Metal device used to render:", m_device->name()->utf8String());
    
    // Create a blank texture atlas
    m_atlas = new TextureAtlas(m_device);
    m_atlas->loadAtlasInMemory();
    
    loadMetal();
}

// MARK: - Destructor
Renderer::~Renderer() {
    m_pipelineState->release();
    m_commandQueue->release();
    m_device->release();
    m_blockFaceVertices->release();
    m_blockFaceIndexBuffer->release();
    m_heap->release();
    m_depthState->release();
    m_instanceDataBuffer->release();
    for ( int i = 0; i < 3; i++ ) {
        m_texture[i]->release();
    }
}

//MARK: - Load metal
void Renderer::loadMetal() {
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();
    
    //MARK: Create textures
    m_texture[0]  = m_atlas->getTextureWithCoordinates(3*16, 9*16); // Undefined texture
    m_texture[1]  = m_atlas->getTextureWithCoordinates(0*16, 3*16); // Grass side
    m_texture[2]  = m_atlas->getTextureWithCoordinates(2*16, 8*16); // Grass top
    m_texture[3]  = m_atlas->getTextureWithCoordinates(0*16, 2*16); // Grass bottom
    m_texture[4]  = m_atlas->getTextureWithCoordinates(1*16, 4*16); // Oak log side
    m_texture[5]  = m_atlas->getTextureWithCoordinates(1*16, 5*16); // Oak log top/bottom
    m_texture[6]  = m_atlas->getTextureWithCoordinates(0*16, 1*16); // Stone all
    m_texture[7]  = m_atlas->getTextureWithCoordinates(0*16, 4*16); // Plank all
    m_texture[8]  = m_atlas->getTextureWithCoordinates(1*16, 10*16); // Leaves all
    m_texture[9]  = m_atlas->getTextureWithCoordinates(0*16, 14*16); // water all
    m_texture[10] = m_atlas->getTextureWithCoordinates(1*16, 2*16); // sand all
    m_texture[11] = m_atlas->getTextureWithCoordinates(3*16, 2*16); // diamond ore all
    m_texture[12] = m_atlas->getTextureWithCoordinates(0*16, 8*16); // tnt side
    m_texture[13] = m_atlas->getTextureWithCoordinates(0*16, 9*16); // tnt top
    m_texture[14] = m_atlas->getTextureWithCoordinates(0*16, 10*16); // tnt bottom
    
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
    m_blockFaceVertices = m_device->newBuffer(quadVertices, // Contents
                                              sizeof(quadVertices), // Length
                                              MTL::ResourceStorageModeShared); // Mode
    m_blockFaceIndexBuffer = m_device->newBuffer(indices,
                                                 sizeof(indices),
                                                 MTL::ResourceStorageModeShared);
    m_instanceDataBuffer = m_device->newBuffer(24 * 16 * 16 * 16 * 6 * sizeof(InstanceData), // Length
                                               MTL::ResourceStorageModeShared); // Mode
    
    Vertex cubeVertices[] =
    {
        //                                         Texture
        //   Positions           Normals         Coordinates
        { { -s, -s, +s }, {  0.f,  0.f,  1.f }, { 0.f, 1.f } }, //  0
        { { +s, -s, +s }, {  0.f,  0.f,  1.f }, { 1.f, 1.f } }, //  1
        { { +s, +s, +s }, {  0.f,  0.f,  1.f }, { 1.f, 0.f } }, //  2
        { { -s, +s, +s }, {  0.f,  0.f,  1.f }, { 0.f, 0.f } }, //  3

        { { +s, -s, +s }, {  1.f,  0.f,  0.f }, { 0.f, 1.f } }, //  4
        { { +s, -s, -s }, {  1.f,  0.f,  0.f }, { 1.f, 1.f } }, //  5
        { { +s, +s, -s }, {  1.f,  0.f,  0.f }, { 1.f, 0.f } }, //  6
        { { +s, +s, +s }, {  1.f,  0.f,  0.f }, { 0.f, 0.f } }, //  7

        { { +s, -s, -s }, {  0.f,  0.f, -1.f }, { 0.f, 1.f } }, //  8
        { { -s, -s, -s }, {  0.f,  0.f, -1.f }, { 1.f, 1.f } }, //  9
        { { -s, +s, -s }, {  0.f,  0.f, -1.f }, { 1.f, 0.f } }, // 10
        { { +s, +s, -s }, {  0.f,  0.f, -1.f }, { 0.f, 0.f } }, // 11

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
    
    uint16_t cubeIndices[] = {
        0,  1,  2,  2,  3,  0, /* front */
        4,  5,  6,  6,  7,  4, /* right */
        8,  9, 10, 10, 11,  8, /* back */
       12, 13, 14, 14, 15, 12, /* left */
       16, 17, 18, 18, 19, 16, /* top */
       20, 21, 22, 22, 23, 20, /* bottom */
    };
    
    m_cubeVertices = m_device->newBuffer(cubeVertices, // Contents
                                         sizeof(cubeVertices), // Length
                                         MTL::ResourceStorageModeShared); // Mode
    m_cubeIndexBuffer = m_device->newBuffer(cubeIndices,
                                            sizeof(cubeIndices),
                                            MTL::ResourceStorageModeShared);
    
    // Assume a maximum of 1000 cubes, or about 500 entities?
    // Normally there would some system where we reallocate a new buffer if we need more space
    m_cubeInstanceDatBuffer = m_device->newBuffer(1000 * sizeof(InstanceData), // Length
                                                  MTL::ResourceStorageModeShared); // Mode
    
    // Create the buffer and the perspective matrix once
    m_cameraDataBuffer = m_device->newBuffer(sizeof(CameraData), MTL::ResourceStorageModeShared);
    
    createHeap();
    moveResourcesToHeap();
    
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
    
    for ( int i = 0; i < TEXTURE_COUNT; i++ ) {
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

//MARK: - Create a new texture descriptor from an existing texture
MTL::TextureDescriptor *Renderer::newDescriptorFromTexture(MTL::Texture *texture,
                                                           MTL::StorageMode storageMode) {
    MTL::TextureDescriptor *descriptor = MTL::TextureDescriptor::alloc()->init();
    
    descriptor->setTextureType(texture->textureType());
    descriptor->setPixelFormat(texture->pixelFormat());
    descriptor->setWidth(texture->width());
    descriptor->setHeight(texture->height());
    descriptor->setDepth(texture->depth());
    descriptor->setMipmapLevelCount(texture->mipmapLevelCount());
    descriptor->setArrayLength(texture->arrayLength());
    descriptor->setSampleCount(texture->sampleCount());
    descriptor->setStorageMode(storageMode);
    
    return descriptor;
};

//MARK: - Create Heap
void Renderer::createHeap() {
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();
    
    MTL::HeapDescriptor *heapDescriptor = MTL::HeapDescriptor::alloc()->init();
    heapDescriptor->setStorageMode(MTL::StorageModePrivate);
    heapDescriptor->setSize(0);
    
    //Build a descriptor for each texture and calculate the size required to store all the textures in the heap.
    for ( int i = 0; i < TEXTURE_COUNT; i++ ) {
        MTL::TextureDescriptor *descriptor = Renderer::newDescriptorFromTexture(m_texture[i],
                                                                                heapDescriptor->storageMode());
        
        MTL::SizeAndAlign sizeAndAlign = m_device->heapTextureSizeAndAlign(descriptor);
        sizeAndAlign.size += (sizeAndAlign.size & (sizeAndAlign.align - 1)) + sizeAndAlign.align;
        heapDescriptor->setSize(heapDescriptor->size() + sizeAndAlign.size);
        
        descriptor->release();
    }
    
    m_heap = m_device->newHeap(heapDescriptor);
    
    heapDescriptor->release();
    pPool->release();
};

//MARK: - Move resources to heap
void Renderer::moveResourcesToHeap() {
    NS::AutoreleasePool *pPool = NS::AutoreleasePool::alloc()->init();
    
    // Create a command buffer and blit encoder to copy data from existing resources
    // to resources created from the heap.
    MTL::CommandBuffer *commandBuffer = m_commandQueue->commandBuffer();
    commandBuffer->setLabel(AAPLSTR("Heap copy command buffer"));
    
    MTL::BlitCommandEncoder *blitEncoder = commandBuffer->blitCommandEncoder();
    blitEncoder->setLabel(AAPLSTR("Heap transfer blit encoder"));
    
    // Create new textures from the heap and copy the contents.
    for ( int i = 0; i < TEXTURE_COUNT; i++ ) {
        MTL::TextureDescriptor *descriptor = Renderer::newDescriptorFromTexture(m_texture[i],
                                                                                m_heap->storageMode());
        
        MTL::Texture *heapTexture = m_heap->newTexture(descriptor);
        heapTexture->setLabel(m_texture[i]->label());
        
        MTL::Region region = MTL::Region::Make2D(0, 0, m_texture[i]->width(), m_texture[i]->height());
        for ( int level = 0; level < m_texture[i]->mipmapLevelCount(); level++ ) {
            for ( int slice = 0; slice < m_texture[i]->arrayLength(); slice++ ) {
                blitEncoder->copyFromTexture(m_texture[i], // sourceTexture
                                             slice, // sourceSlice
                                             level, // sourceLevel
                                             region.origin, // sourceOrigin
                                             region.size, // sourceSize
                                             heapTexture, // toTexture
                                             slice, // destinationSlice
                                             level, // destinationLevel
                                             region.origin); // destinationTexture
            }
            
            region.size.width /= 2;
            region.size.height /= 2;
            if(region.size.width == 0) region.size.width = 1;
            if(region.size.height == 0) region.size.height = 1;
        }
        
        descriptor->release();
        m_texture[i]->release();
        m_texture[i] = heapTexture;
    }
    
    blitEncoder->endEncoding();
    commandBuffer->commit();
    
//    blitEncoder->release();
//    commandBuffer->release();
    pPool->release();
};

// MARK: - Window size will change method
void Renderer::windowSizeWillChange(unsigned int width, unsigned int height) {
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();
    
    m_windowSize.x = width;
    m_windowSize.y = height;
    CameraData *cameraData = reinterpret_cast<CameraData *>(m_cameraDataBuffer->contents());
    cameraData->perspectiveTranform = Math3D::makePerspective(90.f * M_PI / 180.f, m_windowSize.x / m_windowSize.y, 0.01f, 10.f*100);
    
    pPool->release();
};

// MARK: - Draw method
void Renderer::draw(std::vector<std::shared_ptr<Entity>>& entities,
                    MTL::RenderPassDescriptor *currentRPD,
                    MTL::Drawable* currentDrawable) {
    if ( m_instanceCount == 0 ) {
        return;
    }
    
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();
    
    using namespace simd;
    
    // Create a new command buffer for each render pass to the current drawable.
    MTL::CommandBuffer *commandBuffer = m_commandQueue->commandBuffer();
    commandBuffer->setLabel(AAPLSTR("WorldDrawingCommand"));
    
    // Wait for the instance buffer to be available
    m_gpuMutex.lock();
    commandBuffer->addCompletedHandler( ^void( MTL::CommandBuffer* pCmd ) {
        m_gpuMutex.unlock();
    });
    
    int cubeCount = 0;
    InstanceData *cubes = reinterpret_cast<InstanceData *>(m_cubeInstanceDatBuffer->contents());
    for ( auto entity: entities ) {
        for ( auto component: entity->components() ) {
            cubes[cubeCount].transform = entity->transformMatrix() * component->transformMatrix();
            cubes[cubeCount].normalTransform = Math3D::discardTranslation(cubes[cubeCount].transform);
            cubes[cubeCount].textureId = 3;
            cubes[cubeCount].highlighted = false;
            
            cubeCount++;
        }
    }
    
    // MARK: World transformations
    CameraData *cameraData = reinterpret_cast<CameraData *>(m_cameraDataBuffer->contents());
    cameraData->worldTranform = Math3D::makeXRotate4x4(-cameraPitch) * Math3D::makeYRotate4x4(-cameraYaw) * Math3D::makeTranslate( -cameraPosition );
    cameraData->worldNormalTranform = Math3D::discardTranslation(cameraData->worldTranform);
    
    // MARK: Configure draw command
    if ( currentRPD != nullptr ) {
        // Create a render command encoder.
        MTL::RenderCommandEncoder *renderEncoder = commandBuffer->renderCommandEncoder(currentRPD);
        
        // Set the region of the drawable to draw into.
        MTL::Viewport viewPort = {0.0, 0.0, (double) m_windowSize.x, (double) m_windowSize.y, 0.0, 1.0};
        renderEncoder->setViewport(viewPort);
        
        // Indicate to Metal that a GPU heap will be used.
        renderEncoder->useHeap(m_heap);
        
        renderEncoder->setRenderPipelineState(m_pipelineState);
        renderEncoder->setDepthStencilState(m_depthState);
        
        // Pass in the parameter data for cube faces.
        renderEncoder->setVertexBuffer(m_blockFaceVertices, 0, VertexInputIndexVertices);
        renderEncoder->setVertexBuffer(m_instanceDataBuffer, 0, VertexInputIndexInstanceData);
        renderEncoder->setVertexBuffer(m_cameraDataBuffer, 0, VertexInputIndexCameraData);
        renderEncoder->setFragmentBuffer(m_fragmentShaderArgBuffer, 0, FragmentInputIndexTextures);
        renderEncoder->setFragmentBuffer(m_cameraDataBuffer, 0, FragmentInputIndexCameraData);
        
        // Set modes for 3D rendering
        renderEncoder->setCullMode( MTL::CullModeBack );
        renderEncoder->setFrontFacingWinding( MTL::Winding::WindingCounterClockwise );
        
        // Draw our cube faces
        renderEncoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, // Primitive type
                                             6, // Index count
                                             MTL::IndexType::IndexTypeUInt16, // Index type
                                             m_blockFaceIndexBuffer, // Index buffer
                                             0, // Index buffer offset
                                             m_instanceCount); // Instance count
        
        // Reencode paramaters for cube drawing
        renderEncoder->setVertexBuffer(m_cubeVertices, 0, VertexInputIndexVertices);
        renderEncoder->setVertexBuffer(m_cubeInstanceDatBuffer, 0, VertexInputIndexInstanceData);
        
        // Draw our cubes
        renderEncoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, // Primitive type
                                             36, // Index count
                                             MTL::IndexType::IndexTypeUInt16, // Index type
                                             m_cubeIndexBuffer, // Index buffer
                                             0, // Index buffer offset
                                             cubeCount); // Instance count
        
        renderEncoder->endEncoding();
        commandBuffer->presentDrawable(currentDrawable);
    }
    
    commandBuffer->commit();
    
    pPool->release();
}

//MARK: - InstanceDataBuffer getter
InstanceData *Renderer::instanceBuffer() { 
    return reinterpret_cast<InstanceData *>(m_instanceDataBuffer->contents());
}

//MARK: - Instance count getter
int *Renderer::instanceCount() {
    return &m_instanceCount;
}

//MARK: - Remove instance method
void Renderer::removeInstanceAt(int index) {
    InstanceData *buffer = reinterpret_cast<InstanceData *>(m_instanceDataBuffer->contents());
    buffer[index] = buffer[m_instanceCount - 1];
    
    // Get the block we moved the instance of
    Block *b = World::shared()->getBlockAt(buffer[index].blockCoordinates.x,
                                           buffer[index].blockCoordinates.y,
                                           buffer[index].blockCoordinates.z);
    
    // Search for the face with the old index and update it to the new index.
    if ( b != nullptr ) {
        for ( int i = 0; i < 6; i++ ) {
            if ( b->faceIndices[i] == m_instanceCount-1 ) {
                b->faceIndices[i] = index;
                break;
            }
        }
    }
    
    --m_instanceCount; 
};


