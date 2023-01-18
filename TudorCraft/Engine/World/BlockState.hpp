//
//  Block.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 18.11.2022.
//

#ifndef Block_hpp
#define Block_hpp

#include <iostream>
#include <unordered_map>

/// Holds details of any block in the game.
class BlockState {
public:
    /// Getter for the global pallete.
    static std::unordered_map<int, BlockState*>& GlobalPallete();
    
    // The blocks currently supported:
    static BlockState* GrassBlock();
    static BlockState* DirtBlock();
    static BlockState* StoneBlock();
    static BlockState* OakLogBlock();
    static BlockState* OakLeavesBlock();
    static BlockState* WaterBlock();
    static BlockState* OakPlankBlock();
    static BlockState* UndefinedBlock();
    static BlockState* SandBlock();
    static BlockState* TNTBlock();
    static BlockState* DiamondOreBlock();
    
    // Getters:
    int frontTexture() {
        return m_frontTextureIndex;
    };
    
    int rightTexture() {
        return m_rightTextureIndex;
    };
    
    int backTexture() {
        return m_backTextureIndex;
    }
    
    int leftTexture() {
        return m_leftTextureIndex;
    }
    
    int bottomTexture() {
        return m_bottomTextureIndex;
    }
    
    int topTexture() {
        return m_topTextureIndex;
    }
    
    int protocolId() {
        return m_protocolId;
    }
    
private:
    /// Indicates whether or not the block is collidable.
    bool m_collidable;
    
    /// Indicates wheter or not the block is opaque.
    bool m_opaque;
    
    // Texture ids for each face of the block.
    int m_frontTextureIndex;
    int m_rightTextureIndex;
    int m_backTextureIndex;
    int m_leftTextureIndex;
    int m_bottomTextureIndex;
    int m_topTextureIndex;
    
    /// The identifier that the original Minecraft uses to locate its resources.
    std::string m_identifier;
    
    /// The id used by the Minecraft Protocol when sending chunks of blocks.
    int m_globalPaletteId;
    
    /// The id used by the Minecraft Protocol to refer to items and such.
    int m_protocolId;
    
    //MARK: Global palette
    static std::unordered_map<int, BlockState*> m_globalPallete;
    
    //MARK: Default blocks
    static BlockState *minecraft_grass_block;
    static BlockState *minecraft_dirt_block;
    static BlockState *minecraft_stone_block;
    static BlockState *minecraft_oak_log_block;
    static BlockState *minecraft_oak_leaves_block;
    static BlockState *minecraft_water_block;
    static BlockState *minecraft_oak_plank_block;
    static BlockState *undefined_block;
    static BlockState *minecraft_sand_block;
    static BlockState *minecraft_tnt_block;
    static BlockState *minecraft_diamond_ore_block;
};

#endif /* Block_hpp */
