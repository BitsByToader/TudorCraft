//
//  Block.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 18.11.2022.
//

#include "BlockState.hpp"

//MARK: - GrassBlock
BlockState *BlockState::minecraft_grass_block = nullptr;
BlockState* BlockState::GrassBlock() {
    if ( minecraft_grass_block == nullptr ) {
        minecraft_grass_block = new BlockState();
        
        minecraft_grass_block->m_identifier = "minecraft:grass_block";
        minecraft_grass_block->m_globalPaletteId = 9;
        minecraft_grass_block->m_protocolId = 14;
        minecraft_grass_block->m_collidable = true;
        minecraft_grass_block->m_opaque = true;
        
        minecraft_grass_block->m_frontTextureIndex  = 1;
        minecraft_grass_block->m_leftTextureIndex   = 1;
        minecraft_grass_block->m_backTextureIndex   = 1;
        minecraft_grass_block->m_rightTextureIndex  = 1;
        minecraft_grass_block->m_bottomTextureIndex = 3;
        minecraft_grass_block->m_topTextureIndex    = 2;
    }
    
    return minecraft_grass_block;
};

//MARK: - DirtBlock
BlockState *BlockState::minecraft_dirt_block = nullptr;
BlockState* BlockState::DirtBlock() {
    if ( minecraft_dirt_block == nullptr ) {
        minecraft_dirt_block = new BlockState();
        
        minecraft_dirt_block->m_identifier = "minecraft:dirt_block";
        minecraft_dirt_block->m_globalPaletteId = 10;
        minecraft_dirt_block->m_protocolId = 15;
        minecraft_dirt_block->m_collidable = true;
        minecraft_dirt_block->m_opaque = true;
        
        minecraft_dirt_block->m_frontTextureIndex  = 3;
        minecraft_dirt_block->m_leftTextureIndex   = 3;
        minecraft_dirt_block->m_backTextureIndex   = 3;
        minecraft_dirt_block->m_rightTextureIndex  = 3;
        minecraft_dirt_block->m_bottomTextureIndex = 3;
        minecraft_dirt_block->m_topTextureIndex    = 3;
    }
    
    return minecraft_dirt_block;
};

//MARK: - StoneBlock
BlockState *BlockState::minecraft_stone_block = nullptr;
BlockState* BlockState::StoneBlock() {
    if ( minecraft_stone_block == nullptr ) {
        minecraft_stone_block = new BlockState();
        
        minecraft_stone_block->m_identifier = "minecraft:stone_block";
        minecraft_stone_block->m_globalPaletteId = 1;
        minecraft_stone_block->m_protocolId = 1;
        minecraft_stone_block->m_collidable = true;
        minecraft_stone_block->m_opaque = true;
        
        minecraft_stone_block->m_frontTextureIndex  = 6;
        minecraft_stone_block->m_leftTextureIndex   = 6;
        minecraft_stone_block->m_backTextureIndex   = 6;
        minecraft_stone_block->m_rightTextureIndex  = 6;
        minecraft_stone_block->m_bottomTextureIndex = 6;
        minecraft_stone_block->m_topTextureIndex    = 6;
    }
    
    return minecraft_stone_block;
};

//MARK: - OagLog
BlockState *BlockState::minecraft_oak_log_block = nullptr;
BlockState* BlockState::OakLogBlock() {
    if ( minecraft_oak_log_block == nullptr ) {
        minecraft_oak_log_block = new BlockState();
        
        minecraft_oak_log_block->m_identifier = "minecraft:oak_log";
        minecraft_oak_log_block->m_globalPaletteId = 117;
        minecraft_oak_log_block->m_protocolId = 104;
        minecraft_oak_log_block->m_collidable = true;
        minecraft_oak_log_block->m_opaque = true;
        
        minecraft_oak_log_block->m_frontTextureIndex  = 4;
        minecraft_oak_log_block->m_leftTextureIndex   = 4;
        minecraft_oak_log_block->m_backTextureIndex   = 4;
        minecraft_oak_log_block->m_rightTextureIndex  = 4;
        minecraft_oak_log_block->m_bottomTextureIndex = 5;
        minecraft_oak_log_block->m_topTextureIndex    = 5;
    }
    
    return minecraft_oak_log_block;
};

//MARK: - OagLeaves
BlockState *BlockState::minecraft_oak_leaves_block = nullptr;
BlockState* BlockState::OakLeavesBlock() {
    if ( minecraft_oak_leaves_block == nullptr ) {
        minecraft_oak_leaves_block = new BlockState();
        
        minecraft_oak_leaves_block->m_identifier = "minecraft:oak_leaves";
        minecraft_oak_leaves_block->m_globalPaletteId = 206;
        minecraft_oak_leaves_block->m_protocolId = 142;
        minecraft_oak_leaves_block->m_collidable = true;
        minecraft_oak_leaves_block->m_opaque = true;
        
        minecraft_oak_leaves_block->m_frontTextureIndex  = 8;
        minecraft_oak_leaves_block->m_leftTextureIndex   = 8;
        minecraft_oak_leaves_block->m_backTextureIndex   = 8;
        minecraft_oak_leaves_block->m_rightTextureIndex  = 8;
        minecraft_oak_leaves_block->m_bottomTextureIndex = 8;
        minecraft_oak_leaves_block->m_topTextureIndex    = 8;
    }
    
    return minecraft_oak_leaves_block;
};

//MARK: - WaterBlock
BlockState *BlockState::minecraft_water_block = nullptr;
BlockState* BlockState::WaterBlock() {
    if ( minecraft_water_block == nullptr ) {
        minecraft_water_block = new BlockState();
        
        minecraft_water_block->m_identifier = "minecraft:water";
        minecraft_water_block->m_globalPaletteId = 75;
        minecraft_water_block->m_protocolId = -1;
        minecraft_water_block->m_collidable = true;
        minecraft_water_block->m_opaque = true;
        
        minecraft_water_block->m_frontTextureIndex  = 9;
        minecraft_water_block->m_leftTextureIndex   = 9;
        minecraft_water_block->m_backTextureIndex   = 9;
        minecraft_water_block->m_rightTextureIndex  = 9;
        minecraft_water_block->m_bottomTextureIndex = 9;
        minecraft_water_block->m_topTextureIndex    = 9;
    }
    
    return minecraft_water_block;
};

//MARK: - PlankBlock
BlockState *BlockState::minecraft_oak_plank_block = nullptr;
BlockState* BlockState::OakPlankBlock() {
    if ( minecraft_oak_plank_block == nullptr ) {
        minecraft_oak_plank_block = new BlockState();
        
        minecraft_oak_plank_block->m_identifier = "minecraft:oak_plank";
        minecraft_oak_plank_block->m_globalPaletteId = 15;
        minecraft_oak_plank_block->m_protocolId = 23;
        minecraft_oak_plank_block->m_collidable = true;
        minecraft_oak_plank_block->m_opaque = true;
        
        minecraft_oak_plank_block->m_frontTextureIndex  = 7;
        minecraft_oak_plank_block->m_leftTextureIndex   = 7;
        minecraft_oak_plank_block->m_backTextureIndex   = 7;
        minecraft_oak_plank_block->m_rightTextureIndex  = 7;
        minecraft_oak_plank_block->m_bottomTextureIndex = 7;
        minecraft_oak_plank_block->m_topTextureIndex    = 7;
    }
    
    return minecraft_oak_plank_block;
};

//MARK: - SandBlock
BlockState *BlockState::minecraft_sand_block = nullptr;
BlockState* BlockState::SandBlock() {
    if ( minecraft_sand_block == nullptr ) {
        minecraft_sand_block = new BlockState();
        
        minecraft_sand_block->m_identifier = "minecraft:sand";
        minecraft_sand_block->m_globalPaletteId = 107;
        minecraft_sand_block->m_protocolId = 40;
        minecraft_sand_block->m_collidable = true;
        minecraft_sand_block->m_opaque = true;
        
        minecraft_sand_block->m_frontTextureIndex  = 10;
        minecraft_sand_block->m_leftTextureIndex   = 10;
        minecraft_sand_block->m_backTextureIndex   = 10;
        minecraft_sand_block->m_rightTextureIndex  = 10;
        minecraft_sand_block->m_bottomTextureIndex = 10;
        minecraft_sand_block->m_topTextureIndex    = 10;
    }
    
    return minecraft_sand_block;
};

//MARK: - DiamondOreBlock
BlockState *BlockState::minecraft_diamond_ore_block = nullptr;
BlockState* BlockState::DiamondOreBlock() {
    if ( minecraft_diamond_ore_block == nullptr ) {
        minecraft_diamond_ore_block = new BlockState();
        
        minecraft_diamond_ore_block->m_identifier = "minecraft:diamond_ore";
        minecraft_diamond_ore_block->m_globalPaletteId = 3608;
        minecraft_diamond_ore_block->m_protocolId = 57;
        minecraft_diamond_ore_block->m_collidable = true;
        minecraft_diamond_ore_block->m_opaque = true;
        
        minecraft_diamond_ore_block->m_frontTextureIndex  = 11;
        minecraft_diamond_ore_block->m_leftTextureIndex   = 11;
        minecraft_diamond_ore_block->m_backTextureIndex   = 11;
        minecraft_diamond_ore_block->m_rightTextureIndex  = 11;
        minecraft_diamond_ore_block->m_bottomTextureIndex = 11;
        minecraft_diamond_ore_block->m_topTextureIndex    = 11;
    }
    
    return minecraft_diamond_ore_block;
};

//MARK: - TNTBlock
BlockState *BlockState::minecraft_tnt_block = nullptr;
BlockState* BlockState::TNTBlock() {
    if ( minecraft_tnt_block == nullptr ) {
        minecraft_tnt_block = new BlockState();
        
        minecraft_tnt_block->m_identifier = "minecraft:tnt";
        minecraft_tnt_block->m_globalPaletteId = 1685;
        minecraft_tnt_block->m_protocolId = 629;
        minecraft_tnt_block->m_collidable = true;
        minecraft_tnt_block->m_opaque = true;
        
        minecraft_tnt_block->m_frontTextureIndex  = 12;
        minecraft_tnt_block->m_leftTextureIndex   = 12;
        minecraft_tnt_block->m_backTextureIndex   = 12;
        minecraft_tnt_block->m_rightTextureIndex  = 12;
        minecraft_tnt_block->m_bottomTextureIndex = 14;
        minecraft_tnt_block->m_topTextureIndex    = 13;
    }
    
    return minecraft_tnt_block;
};

//MARK: - UndefinedBlock
BlockState *BlockState::undefined_block = nullptr;
BlockState* BlockState::UndefinedBlock() {
    if ( undefined_block == nullptr ) {
        undefined_block = new BlockState();
        
        undefined_block->m_identifier = "";
        undefined_block->m_globalPaletteId = -1;
        undefined_block->m_protocolId = -1;
        undefined_block->m_collidable = true;
        undefined_block->m_opaque = true;
        
        undefined_block->m_frontTextureIndex  = 0;
        undefined_block->m_leftTextureIndex   = 0;
        undefined_block->m_backTextureIndex   = 0;
        undefined_block->m_rightTextureIndex  = 0;
        undefined_block->m_bottomTextureIndex = 0;
        undefined_block->m_topTextureIndex    = 0;
    }
    
    return undefined_block;
};

//MARK: - GlobalPallete
std::unordered_map<int, BlockState*> BlockState::m_globalPallete;
std::unordered_map<int, BlockState*>& BlockState::GlobalPallete() {
    if ( m_globalPallete.empty() ) {
        m_globalPallete[0] = nullptr;
        m_globalPallete[10546] = nullptr;
        m_globalPallete[10547] = nullptr;
        
        m_globalPallete[GrassBlock()->m_globalPaletteId] = GrassBlock();
        m_globalPallete[DirtBlock()->m_globalPaletteId] = DirtBlock();
        m_globalPallete[StoneBlock()->m_globalPaletteId] = StoneBlock();
        
        m_globalPallete[OakLogBlock()->m_globalPaletteId] = OakLogBlock();
        m_globalPallete[118] = OakLogBlock();
        m_globalPallete[119] = OakLogBlock();
        
        for ( int i = 206; i <= 233; i++ ) {
            m_globalPallete[i] = OakLeavesBlock();
        }
        
        for ( int i = 75; i <= 90; i++ ) {
            m_globalPallete[i] = WaterBlock();
        }
        
        m_globalPallete[OakPlankBlock()->m_globalPaletteId] = OakPlankBlock();
        m_globalPallete[SandBlock()->m_globalPaletteId] = SandBlock();
        
        m_globalPallete[DiamondOreBlock()->m_globalPaletteId] = DiamondOreBlock();
        m_globalPallete[TNTBlock()->m_globalPaletteId] = TNTBlock();
    }
    
    return m_globalPallete;
};
