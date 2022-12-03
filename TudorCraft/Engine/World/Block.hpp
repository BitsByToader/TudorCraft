//
//  Block.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 18.11.2022.
//

#ifndef Block_hpp
#define Block_hpp

#include <iostream>

class Block {
public:
    static Block* Dirt();
    
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
    
    //MARK: Default blocks
    static Block *minecraft_dirt;
};

#endif /* Block_hpp */
