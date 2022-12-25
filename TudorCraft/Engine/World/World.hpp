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

class World {
public:
    World();
    
    ~World();
    
    /// Getter for  the`World` singleton.
    static World *shared();
    
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
    
    void calculateMeshes();
    
private:
    static World *m_globalObject;
    
#warning Question: Use an array instead to reduce overhead when accesing a block in the world?
    std::unordered_map<Tuple3D, Chunk *, Tuple3DHash> m_chunks;
};

#endif /* World_hpp */
