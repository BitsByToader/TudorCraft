//
//  World.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 18.11.2022.
//

#ifndef World_hpp
#define World_hpp

#include <unordered_map>
#include <tuple>
#include <functional>
#include <simd/simd.h>

#include "Math3D.hpp"
#include "BlockState.hpp"

// Forward declarations
class Chunk;
class Block;

typedef std::tuple<int, int, int> Tuple3D;

struct Tuple3DHash {
    // Hash function
    size_t operator() ( const Tuple3D& k) const {
        auto [x, y, z] = k;
        
        return Math3D::hash3D(x, y, z);
    }
};

/// Manages, adds/removes from rendering all of the chunks that the game received from the server.
class World {
public:
    World();
    
    ~World();
    
    /// Getter for  the`World` singleton.
    static World *shared();
    
    static void destroySharedObject();
    
    /// Returns the chunk at the given world coordinates.
    /// - Parameters:
    ///   - x: X coordinate
    ///   - y: Y coordinate
    ///   - z: Z coordinate
    /// - Returns: The chunk or `nullptr` if the chunk was not found
    Chunk* getChunkAt(int x, int y, int z);
    
    void loadChunk(Chunk *c);
    
    /// Returns the block at the given coordinates for the loaded chunks.
    /// - Parameters:
    ///   - x: X coordinate of block.
    ///   - y: Y coordinate of block.
    ///   - z: Z coordinate of block.
    /// - Returns: Returns the block in the world, or `nullptr` if the block is an air block or it's not currently loaded.
    Block* getBlockAt(int x, int y, int z);
    
    /// Places the block with the given state in the world at the specified world coordinates.
    /// - Parameters:
    ///   - x: X world coordinate of block
    ///   - y: Y world coordinate
    ///   - z: Z world coordinate
    ///   - state: The new state of the block at the coordinates. Pass `nullptr` to remove a block from the world.
    void placeBlockAt(int x, int y, int z, BlockState *state);
    
    /// Highlights the block.
    /// - Parameters:
    ///   - x: X coordinate of block to highlight.
    ///   - y: Z coordinate of block to highlight.
    ///   - z: Y coordinate of block to highlight.
    void highlightBlock(int x, int y, int z);
    
    /// Removes the highlight from the currently highlighted block.
    void removeHightlight();
    
    /// Currently, this is fine to be left as is, but in the future a getter/settter combo is necessary.
    ///
    /// The center chunk is used to determine what chunks to add/remove from rendering based on the
    /// set render distance. However, we currently don't support this functionality.
    simd::float2 centerChunk = (simd::float2) { 0.f, 0.f };
private:
    static World *m_sharedObject;
    
    std::unordered_map<Tuple3D, Chunk *, Tuple3DHash> m_chunks;
    
    Block *m_highlightedBlock = nullptr;
};

#endif /* World_hpp */
