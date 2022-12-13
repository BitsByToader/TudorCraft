//
//  Block.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 18.11.2022.
//

#include "BlockState.hpp"

BlockState *BlockState::minecraft_grass_block = nullptr;
BlockState* BlockState::GrassBlock() {
    if ( minecraft_grass_block == nullptr ) {
        minecraft_grass_block = new BlockState();
        
        minecraft_grass_block->identifier = "minecraft:grass_block";
        minecraft_grass_block->globalPaletteId = 9;
        minecraft_grass_block->m_collidable = true;
        minecraft_grass_block->m_opaque = true;
        
        minecraft_grass_block->m_frontTextureIndex  = 0;
        minecraft_grass_block->m_leftTextureIndex   = 0;
        minecraft_grass_block->m_backTextureIndex   = 0;
        minecraft_grass_block->m_rightTextureIndex  = 0;
        minecraft_grass_block->m_bottomTextureIndex = 2;
        minecraft_grass_block->m_topTextureIndex    = 1;
    }
    
    return minecraft_grass_block;
};
