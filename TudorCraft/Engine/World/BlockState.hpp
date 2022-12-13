//
//  Block.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 18.11.2022.
//

#ifndef Block_hpp
#define Block_hpp

#include <iostream>

class BlockState {
public:
    static BlockState* GrassBlock();
    
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
    std::string identifier;
    
    /// The id used by the Minecraft Protocol when sending chunks of blocks.
    int globalPaletteId;
    
    //MARK: Default blocks
    static BlockState *minecraft_grass_block;
};

#endif /* Block_hpp */
