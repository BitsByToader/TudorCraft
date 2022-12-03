//
//  Block.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 18.11.2022.
//

#include "Block.hpp"

Block *Block::minecraft_dirt = nullptr;
Block* Block::Dirt() {
    if ( minecraft_dirt == nullptr ) {
        minecraft_dirt = new Block();
        
        minecraft_dirt->identifier = "minecraft:dirt";
        minecraft_dirt->m_collidable = true;
        minecraft_dirt->m_opaque = true;
        
        minecraft_dirt->m_frontTextureIndex  = 0;
        minecraft_dirt->m_leftTextureIndex   = 0;
        minecraft_dirt->m_backTextureIndex   = 0;
        minecraft_dirt->m_rightTextureIndex  = 0;
        minecraft_dirt->m_bottomTextureIndex = 2;
        minecraft_dirt->m_topTextureIndex    = 1;
    }
    
    return minecraft_dirt;
};
