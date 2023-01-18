//
//  Renderer.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 02.11.2022.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include <mutex>
#include <memory>

#include <Metal/Metal.hpp>
#include <simd/simd.h>
#include "ShaderTypes.h"
#include "Entity.hpp"
#include "TextureAtlas.hpp"

/// The voxle renderer responsible  for displaying the world and all its contents to the drawable.
///
/// # Features:
/// - Mesh generation in the `World` to render individual faces instead of the whole cube.
/// - Backface culling to prevent rendering of the faces which face away from the camera.
/// - Indexed and instanced rendering for better memory usage and an easier instance manangement.
/// - Depth ordering on the GPU side to properly display all the faces of the block.
/// - Usage of the GPU heap for fast texture loading between frames.
/// - Memory-leak-free management using (manual) reference counting (from `NS::Object`)  and `NS::AutoReleasePool`
///
/// *NOTE:* We're currently using manual reference counting for all of the memory shared with the GPU. While this is currently fine, we
/// can use `NS::SharedPtr` which are similar to `std::shared_ptr` (but have some optimizations for the memory model used by Apple)
/// to esentially prevent any possible memory leaks.
class Renderer {
public:
    // MARK: Public Methods
    /// Generic constructor for default Renderer (which is what will be used)
    Renderer();
    
    /// Default destructor.
    ~Renderer();
    
    /// Getter for the global Renderer singleton.
    static Renderer *shared();
    
    /// Destoys the shared object.
    ///
    /// **WARNING:  Only use this when there are no other objects holding a pointer to the shared object!**
    static void destroySharedObject();
    
    /// Notifies the renderer that the window size changed.
    /// - Parameters:
    ///   - width: The new width of the window.
    ///   - height: The new height of the window.
    void windowSizeWillChange(unsigned int width, unsigned int height);
    
    
    /// Tells the renderer to draw in the drawable using the render descriptor.
    /// - Parameters:
    ///   - entities: The entities to draw along with the world.
    ///   - currentRPD: Render pass descriptor to use.
    ///   - currentDrawable: Drawable to draw in.
    void draw(std::vector<std::shared_ptr<Entity>>& entities,
              MTL::RenderPassDescriptor *currentRPD,
              MTL::Drawable* currentDrawable);
    
    
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
    
    /// Sets the camera to a place in the world and gives it an orientation.
    /// - Parameters:
    ///   - pos: Position in render coordinates.
    ///   - yaw: Yaw in radians.
    ///   - pitch: Pitch in radians.
    void setCameraInWorld(simd::float3 pos, float yaw, float pitch);
    
    /// The mutex (binary semaphose) used to sync the CPU to the GPU when updating the world to prevent acceses while writing/reading.
    std::mutex m_gpuMutex;
private:
    static Renderer *m_sharedObject;
    
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
    MTL::Texture *m_texture[TEXTURE_COUNT];
    
    /// Vertex data buffer
    MTL::Buffer *m_blockFaceVertices;
    MTL::Buffer *m_cubeVertices;
    
    /// Indices buffer
    MTL::Buffer* m_blockFaceIndexBuffer;
    MTL::Buffer* m_cubeIndexBuffer;
    
    /// Instance data buffer
    MTL::Buffer* m_instanceDataBuffer;
    MTL::Buffer* m_cubeInstanceDatBuffer;
    
    /// Number of instances in any of the data buffers
    int m_instanceCount = 0;
    
    /// Camera data buffer
    MTL::Buffer* m_cameraDataBuffer;
    
    /// Buffer for the fragment shader to store the textures inside of
    MTL::Buffer* m_fragmentShaderArgBuffer;
    
    /// Window size
    vector_uint2 m_windowSize;
    
    /// The atlas we're using to load the textures from.
    TextureAtlas *m_atlas;
    
    /// Camera position and orietantion members.
    simd::float3 m_cameraPosition = (simd::float3) { 0.f, 0.f, 0.f };
    float m_cameraYaw = 0.f;
    float m_cameraPitch = 0.f;
};

#endif /* Renderer_hpp */
