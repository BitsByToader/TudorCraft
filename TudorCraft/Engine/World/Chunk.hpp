//
//  Chunk.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 18.11.2022.
//

#ifndef Chunk_hpp
#define Chunk_hpp

#include "BlockState.hpp"
#include "Renderer.hpp"
#include "ShaderTypes.h"
#include "World.hpp"
#include "Faces.hpp"

#define BLOCK_SIZE 10.f // 1 meter
#define BLOCKS_NUM_X 16
#define BLOCKS_NUM_Y 16
#define BLOCKS_NUM_Z 16
#define BLOCKS_NUM_TOTAL 16*16*16

//MARK: - Block struct
///  Basic struct which holds the state and the indices for the instance data.
struct Block {
    /// The state of the block
    BlockState *state = nullptr;
    
    /// Instance indices for each face of the block.
    int faceIndices[6];
};

//MARK: - Chunk class
/// Smallest form of storage for a MC world, holding blocks in a contiguous manner.
class Chunk {
public:
    /// Default constructor to generate an empty chunk
    Chunk(int x, int y, int z);
    
    /// Destructor.
    ~Chunk();
    
    /// Gets the block in the chunk at the given coordinates.
    /// - Parameters:
    ///   - x: X coordinate of block to get from the chunk
    ///   - y: Y coordinate of block to get from the chunk
    ///   - z: Z coordinate of block to get from the chunk
    ///
    /// - Returns:Returns the specified block or `nullptr` if the block is an air block.
    Block *getBlockAt(int x, int y, int z);
    
    /// Calculates the block faces that need to be rendered in the chunk.
    ///
    /// This method relies on the fact that no previous mesh generation was done on the renderer for this chunk.
    /// If a recalculation is necessary, it's expected that the InstanceData buffer is cleaned beforehand for this chunk.
    /// Otherwise, use the `placeBlockAt` method to properly modify the mesh in place in the buffer.
    ///
    /// - Parameters:
    ///   - renderer: Where the mesh should be generated. Default value is the global `Renderer` singleton.
    void calculateMesh(Renderer *renderer = Renderer::shared());
    
    
    /// Places a block with the `state` in the `world` updating the rendering instances for `renderer`.
    ///
    /// - Warning: The `renderer` and `world` arguments **have** to be the same for every call,
    /// using other worlds and renderers is undefined behaviour!
    ///
    /// - Parameters:
    ///   - x: X relative coordinate in the chunk
    ///   - y: Y relative coordinate in the chunk
    ///   - z: Z relative coordinate in the chunk
    ///   - state: The state of the new block, `nullptr` for an air block.
    ///   - renderer: The renderer which gets updated with the new instances
    ///   - world: The world where we're placing the block in, the same world that contains this chunk.
    void placeBlockAt(int x, int y, int z,
                      BlockState *state,
                      Renderer *renderer = Renderer::shared(),
                      World *world = World::shared());
    
    /// Similar in concept to `placeBlockAt()`, but this method only updates the state in the chunk.
    ///
    /// As such, the change will not be reflected in world or in the rendering in any way. After a series of `setBlockAt` it's expected
    /// to call `calculateMesh()` to modify
    void setBlockAt(int x, int y, int z,
                    BlockState *state);
    
    int x() {
        return m_xCoordinate;
    };
    
    int y() {
        return m_yCoordinate;
    };
    
    int z() {
        return m_zCoordinate;
    };
    
private:
    // These are chunk coordinates, not the block coordinate of the first block!
    int m_xCoordinate;
    int m_yCoordinate;
    int m_zCoordinate;
    
    int m_blockCount = 0;
    Block  *m_blocks;
};

#endif /* Chunk_hpp */
