//
//  Packet.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 02.12.2022.
//

#include "Packet.hpp"

#include "TCPStream.hpp"

#include "Engine.hpp"
#include "World.hpp"
#include "Chunk.hpp"

using namespace MCP;

//MARK: - Handshake
HandshakePacket::HandshakePacket(int protocolVersion, std::string serverAddress, short port) {
    m_protocolVersion = MCP::VarInt(protocolVersion);
    m_port = port;
    m_serverAddress = serverAddress;
};

void HandshakePacket::write(TCPStream *stream) {
    MCP::VarInt packetId(getPacketId());
    MCP::VarInt addressLength((int)m_serverAddress.size());
    MCP::VarInt size(packetId.size() + // Packet Id
                     m_protocolVersion.size() + // Protocol Version
                     addressLength.size() + // Address
                     (int) m_serverAddress.size() + // Address
                     2 + // Port
                     nextState.size() ); // NextState
    
    *stream << &size;
    *stream << &packetId;
    
    *stream << &m_protocolVersion;
    *stream << &m_serverAddress;
    *stream << &m_port;
    *stream << &nextState;
    
    // Don't flush the buffer now because we need to quickly send LoginStart as well
};

//MARK: - Login Start
LoginStartPacket::LoginStartPacket(std::string username) {
    m_username = username;
}

void LoginStartPacket::write(TCPStream *stream) {
    MCP::VarInt packetId(getPacketId());
    MCP::VarInt usernameLength((int)m_username.size());
    MCP::VarInt size(packetId.size() +
                     usernameLength.size() +
                     (int) m_username.size() +
                     2 );// The two false booleans
    
    bool b = false;
    
    *stream << &size;
    *stream << &packetId;
    
    *stream << &m_username;
    *stream << &b;
    *stream << &b;
    
    stream->flushOutput();
};

//MARK: - Login Success
void LoginSuccessPacket::read(TCPStream *stream) {
    MCP::VarInt propertiesCount;
    
    *stream >> &m_uuid;
    *stream >> &m_username;
    *stream >> &propertiesCount;
    
    for ( int i = 0; i < propertiesCount.value(); i++ ) {
        Property p;
        *stream >> &p;
        
        m_properties.push_back(p);
    }
    
    AAPL_PRINT("Username:", m_username);
};

//MARK: - Player Info
void PlayerInfoPacket::read(TCPStream *stream) {
    MCP::VarInt playerCount;
    MCP::VarInt propertyCount;
    
    *stream >> &m_action;
    *stream >> &playerCount;
    
    m_uuids.reserve(playerCount.value());
    
    switch (m_action.value()) {
        case 0: { // Add player
            m_players.reserve(playerCount.value());
            for ( int i = 0; i < playerCount.value(); i++ ) {
                MCP::Uuid uuid;
                *stream >> &uuid;
                m_uuids.push_back(uuid);
                
                Player player;
                
                *stream >> &player.name;
                
                *stream >> &propertyCount;
                for ( int j = 0; j < propertyCount.value(); j++ ) {
                    Property property;
                    
                    *stream >> &property;
                    player.properties.push_back(property);
                }
                
                *stream >> &player.gamemode;
                *stream >> &player.ping;
                *stream >> (int8_t *) &player.hasDisplayName;
                
                if ( player.hasDisplayName )
                    *stream >> &player.displayName;
                
                *stream >> (int8_t *) &player.hasSigData;
                if ( player.hasSigData ) {
                    *stream >> &player.timestamp;
                    *stream >> &player.keyLength;
                    for ( int j = 0; j < player.keyLength.value(); j++ ) {
                        int8_t c;
                        *stream >> &c;
                        player.key.push_back(c);
                    }
                    
                    *stream >> &player.signatureLength;
                    for ( int j = 0; j < player.signatureLength.value(); j++ ) {
                        int8_t c;
                        *stream >> &c;
                        player.signature.push_back(c);
                    }
                }
                
                m_players.push_back(player);
            }
            
            break;
        }
            
        case 1: { // Update gamemode
            MCP::Uuid uuid;
            *stream >> &uuid;
            m_uuids.push_back(uuid);
            
            m_gamemodes.reserve(playerCount.value());
            for ( int i = 0; i < playerCount.value(); i++ ) {
                MCP::Uuid uuid;
                *stream >> &uuid;
                m_uuids.push_back(uuid);
                
                MCP::VarInt gm;
                *stream >> &gm;
                m_gamemodes.push_back(gm);
            }
            
            break;
        }
            
        case 2: { // Update latency
            m_gamemodes.reserve(playerCount.value());
            for ( int i = 0; i < playerCount.value(); i++ ) {
                MCP::Uuid uuid;
                *stream >> &uuid;
                m_uuids.push_back(uuid);
                
                MCP::VarInt ping;
                *stream >> &ping;
                m_pings.push_back(ping);
            }
            
            break;
        }
            
        case 3: { // Update display name
            m_names.reserve(playerCount.value());
            for ( int i = 0; i < playerCount.value(); i++ ) {
                MCP::Uuid uuid;
                *stream >> &uuid;
                m_uuids.push_back(uuid);
                
                NewDisplayName name;
                *stream >> (int8_t *) &name.hasDisplayName;
                if ( name.hasDisplayName )
                    *stream >> &name.displayName;
                
                m_names.push_back(name);
            }
            break;
        }
        case 4: { // Remove player
            for ( int i = 0; i < playerCount.value(); i++ ) {
                MCP::Uuid uuid;
                *stream >> &uuid;
                m_uuids.push_back(uuid);
            }
            break;
        }
            
        default:
            break;
    }
};

//MARK: - Keep Alive
void KeepAlivePacket::write(TCPStream *stream) {
    MCP::VarInt packetId((int) MCP::ServerBoundPlayingPacketTypes::KeepAlive);
    MCP::VarInt packetSize( (int)packetId.size() + sizeof(m_id) );
    
    *stream << &packetSize;
    *stream << &packetId;
    *stream << &m_id;
    
    stream->flushOutput();
};

void KeepAlivePacket::read(TCPStream *stream) {
    *stream >> &m_id;
};

//MARK: - Synchronize Player Position
void SynchronizePlayerPositionPacket::read(TCPStream *stream) {
    *stream >> &m_x;
    *stream >> &m_y;
    *stream >> &m_z;
    
    *stream >> &m_yaw;
    *stream >> &m_pitch;
    
    *stream >> (int8_t *) &m_flags;
    *stream >> &m_teleportId;
    *stream >> (int8_t *) &m_dismountVehicle;
    
    // Confirm the spawn
    MCP::ConfirmTeleportPacket confirmPacket(m_teleportId.value());
    confirmPacket.write(stream);
    
    MCP::SetCreativeModeSlotPacket setslot(0, BlockState::GrassBlock()->protocolId());
    setslot.write(stream);
    
    setslot = MCP::SetCreativeModeSlotPacket (1, BlockState::DirtBlock()->protocolId());
    setslot.write(stream);
    
    setslot = MCP::SetCreativeModeSlotPacket (2, BlockState::StoneBlock()->protocolId());
    setslot.write(stream);
    
    setslot = MCP::SetCreativeModeSlotPacket (3, BlockState::SandBlock()->protocolId());
    setslot.write(stream);
    
    setslot = MCP::SetCreativeModeSlotPacket (4, BlockState::OakPlankBlock()->protocolId());
    setslot.write(stream);
    
    setslot = MCP::SetCreativeModeSlotPacket (5, BlockState::OakLogBlock()->protocolId());
    setslot.write(stream);
    
    setslot = MCP::SetCreativeModeSlotPacket (6, BlockState::OakLeavesBlock()->protocolId());
    setslot.write(stream);
    
    setslot = MCP::SetCreativeModeSlotPacket (7, BlockState::TNTBlock()->protocolId());
    setslot.write(stream);
    
    setslot = MCP::SetCreativeModeSlotPacket (8, BlockState::DiamondOreBlock()->protocolId());
    setslot.write(stream);
    
    MCP::SetHeldItemPacket setheld(0);
    setheld.write(stream);
    
    AAPL_PRINT("Spawn:", m_x, m_y, m_z);
    
    // World coordinates -> Render coordinates transform:
    // We need this because everything needs to be scaled to the blocks' size (10.f).
    // Also, the minecraft world has decreasing Z coordinates instead of increasing Z coordinates
    // so negate the Z component of the entity's location.
    Engine::shared()->spawnPlayer((float)m_x*10,
                                  (float)m_y*10,
                                  -(float)m_z*10,
                                  -(m_yaw+90.f) * M_PI / 180.f,
                                  m_pitch * M_PI / 180.f);
};

//MARK: - Center Chunk
void CenterChunkPacket::read(TCPStream *stream) {
    *stream >> &m_chunkX;
    *stream >> &m_chunkZ;
    
    AAPL_PRINT("Center:", m_chunkX.value(), m_chunkZ.value());
    World::shared()->centerChunk = { (float)m_chunkX.value(), (float)m_chunkZ.value() };
};

//MARK: - Chunk Data
void ChunkDataPacket::read(TCPStream *stream) {
    *stream >> &m_chunkX;
    *stream >> &m_chunkZ;
    
    *stream >> &m_heightmap;
    *stream >> &m_dataSize;
    
    for ( int chunkY = 0; chunkY < 384/16; chunkY++ ) {
        ChunkSection section;
        
        *stream >> &section.blockCount;
        
        // BlockStates Pallete
        *stream >> &section.blockStates.bitsPerEntry;
        
        if ( section.blockStates.bitsPerEntry == 0 ) {
            *stream >> &section.blockStates.palleteValue;
        } else {
            *stream >> &section.blockStates.palleteLength;
            
            section.blockStates.pallete.reserve(section.blockStates.palleteLength.value());
            for ( int i = 0; i < section.blockStates.palleteLength.value(); i++ ) {
                VarInt v;
                *stream >> &v;
                section.blockStates.pallete.push_back(v);
            }
        }
        
        *stream >> &section.blockStates.dataArrayLength;
        section.blockStates.dataArray.reserve(section.blockStates.dataArrayLength.value());
        for ( int i = 0; i < section.blockStates.dataArrayLength.value(); i++ ) {
            int64_t l;
            *stream >> &l;
            section.blockStates.dataArray.push_back(l);
        }
        // BlockStates Pallete
        
        // Biomes Pallete
        *stream >> &section.biomes.bitsPerEntry;
        
        if ( section.biomes.bitsPerEntry == 0 ) {
            *stream >> &section.biomes.palleteValue;
        } else {
            *stream >> &section.biomes.palleteLength;
            
            section.biomes.pallete.reserve(section.biomes.palleteLength.value());
            for ( int i = 0; i < section.biomes.palleteLength.value(); i++ ) {
                VarInt v;
                *stream >> &v;
                section.biomes.pallete.push_back(v);
            }
        }
        
        *stream >> &section.biomes.dataArrayLength;
        section.biomes.dataArray.reserve(section.biomes.dataArrayLength.value());
        for ( int i = 0; i < section.biomes.dataArrayLength.value(); i++ ) {
            int64_t l;
            *stream >> &l;
            section.biomes.dataArray.push_back(l);
        }
        // Biomes Pallete
        
        
        m_data.push_back(section);
    };
    
    stream->skipRestOfPacket();
};

void ChunkDataPacket::updateGame() {
    simd::float2 pos = { (float)m_chunkX, (float)m_chunkZ };
    simd::float2 center = World::shared()->centerChunk;
    
    if ( simd::distance(center, pos) > 3 )
        return;
    
    std::unordered_map<int, BlockState*> pallete = BlockState::GlobalPallete();
    World *w = World::shared();
    
    Renderer::shared()->m_gpuMutex.lock();
    
    // For every chunk in the section
    for ( int chunkY = 0; chunkY < 384/16; chunkY++ ) {
        MCP::ChunkDataPacket::ChunkSection section = m_data.at(chunkY);
        MCP::ChunkDataPacket::ChunkSection::PalletedContainer blocks = section.blockStates;
        
        Chunk *newChunk = new Chunk(m_chunkX, chunkY-4, m_chunkZ);
        w->loadChunk(newChunk);
        
        int blockCount = section.blockCount;
        
        if ( blocks.bitsPerEntry == 0 || blockCount == 0 ) {
            continue;
        }
        
        int longCount = 0;
        int longShiftCount = 0;
        int64_t longToUse = blocks.dataArray[0];
        int64_t mask = pow(2, blocks.bitsPerEntry) - 1;
        
        // For every block in the chunk
        for ( int y = 0; y < 16; y++ ) {
            for ( int z = 0; z < 16; z++ ) {
                for ( int x = 0; x < 16; x++ ) {
                    int64_t indexInPallete = longToUse & mask;
                    
                    longToUse >>= blocks.bitsPerEntry;
                    
                    longShiftCount++;
                    
                    if ( longShiftCount == 64/blocks.bitsPerEntry ) {
                        longShiftCount = 0;
                        longCount++;
                        longToUse = blocks.dataArray[longCount];
                    }
                    
                    try {
                        newChunk->placeBlockAt(x, y, z,
                                               pallete.at(blocks.pallete[indexInPallete].value()));
                    } catch(...) {
                        newChunk->placeBlockAt(x, y, z, BlockState::UndefinedBlock());
                    }
                }
            }
        }
    }
    
    Renderer::shared()->m_gpuMutex.unlock();
};

//MARK: - Block Update
void BlockUpdatePacket::read(TCPStream *stream) {
    *stream >> &m_blockPosition;
    *stream >> &m_newState;
};

void BlockUpdatePacket::updateGame() {
    int x = ((int64_t) m_blockPosition) >> 38;
    int y = m_blockPosition << 52 >> 52;
    int z = m_blockPosition << 26 >> 38;
    
    try {
        World::shared()->placeBlockAt(x, y, z, BlockState::GlobalPallete().at(m_newState.value()));
    } catch (...) {
        World::shared()->placeBlockAt(x, y, z, BlockState::UndefinedBlock());
    }
};

//MARK: - Spawn Player
void SpawnPlayerPacket::read(TCPStream* stream) {
    *stream >> &m_entityId;
    *stream >> &m_playerUuid;
    *stream >> &m_x;
    *stream >> &m_y;
    *stream >> &m_z;
    *stream >> &m_yaw;
    *stream >> &m_pitch;
};

void SpawnPlayerPacket::updateGame() {
    std::shared_ptr<MobEntity> mob = std::make_shared<MobEntity>(m_x*10, m_y*10, -m_z*10);
    
    m_pitch = (m_pitch * 360.f/256.f) * M_PI / 180.f;
    m_yaw = (m_yaw * 360.f/256.f) * M_PI / 180.f;
    
    mob->setRotation(0.f, m_yaw);
    mob->cameraBoundComponent->setRotation(m_pitch, m_yaw, 0.f);
    
    Engine::shared()->addEntity(m_entityId.value(), mob);
};

//MARK: - Spawn Entity
void SpawnEntityPacket::read(TCPStream* stream) {
    *stream >> &m_entityId;
    *stream >> &m_entityUuid;
    *stream >> &m_type;
    *stream >> &m_x;
    *stream >> &m_y;
    *stream >> &m_z;
    *stream >> &m_pitch;
    *stream >> &m_yaw;
    *stream >> &m_headYaw;
    *stream >> &m_data;
    *stream >> &m_velocityX;
    *stream >> &m_velocityY;
    *stream >> &m_velocityZ;
};

void SpawnEntityPacket::updateGame() {
    std::shared_ptr<MobEntity> mob = std::make_shared<MobEntity>(m_x*10, m_y*10, -m_z*10);
    
    m_pitch = (m_pitch * 360.f/256.f) * M_PI / 180.f;
    m_yaw = (m_yaw * 360.f/256.f) * M_PI / 180.f;
    m_headYaw = (m_headYaw * 360.f/256.f) * M_PI / 180.f;
    
    mob->setRotation(0.f, m_yaw);
    mob->cameraBoundComponent->setRotation(m_pitch, m_headYaw, 0.f);
    mob->setSpeed(m_velocityX, m_velocityY, m_velocityZ);
    
    Engine::shared()->addEntity(m_entityId.value(), mob);
};

//MARK: - Remove Entities
void RemoveEntitiesPacket::read(TCPStream *stream) {
    *stream >> &m_count;
    
    m_ids.reserve(m_count.value());
    for ( int i = 0; i < m_count.value(); i++ ) {
        MCP::VarInt toRead;
        *stream >> &toRead;
        m_ids.push_back(toRead);
    }
};

void RemoveEntitiesPacket::updateGame() {
    Engine *e = Engine::shared();
    for ( auto& i: m_ids ) {
        e->removeEntity(i.value());
    }
}

//MARK: - Set Entity Velocity
void SetEntityVelocityPacket::read(TCPStream *stream) {
    *stream >> &m_entityId;
    *stream >> &m_velocityX;
    *stream >> &m_velocityY;
    *stream >> &m_velocityZ;
};

void SetEntityVelocityPacket::updateGame() {
    std::shared_ptr<MobEntity> e = Engine::shared()->getEntity(m_entityId.value());
    if ( e != nullptr ) {
        e->setSpeed(m_velocityX/8000/50/10, m_velocityY/8000/50/10, m_velocityZ/8000/50/10);
    }
};

//MARK: - Update Entity Position
void UpdateEntityPositionPacket::read(TCPStream *stream) {
    *stream >> &m_entityId;
    *stream >> &m_deltaX;
    *stream >> &m_deltaY;
    *stream >> &m_deltaZ;
    *stream >> &m_onGround;
}

void UpdateEntityPositionPacket::updateGame() {
    std::shared_ptr<MobEntity> e = Engine::shared()->getEntity(m_entityId.value());
    if ( e != nullptr ) {
        simd::float3 prev = e->position();
        e->setPosition(m_deltaX/4096.f*10.f + prev.x,
                       m_deltaY/4096.f*10.f + prev.y,
                       -m_deltaZ/4096.f*10.f + prev.z);
        e->onGround = m_onGround;
    }
};

//MARK: - Update Entity Position And Rotation
void UpdateEntityPositionAndRotationPacket::read(TCPStream *stream) {
    *stream >> &m_entityId;
    *stream >> &m_deltaX;
    *stream >> &m_deltaY;
    *stream >> &m_deltaZ;
    *stream >> &m_yaw;
    *stream >> &m_pitch;
    *stream >> &m_onGround;
}

void UpdateEntityPositionAndRotationPacket::updateGame() {
    std::shared_ptr<MobEntity> e = Engine::shared()->getEntity(m_entityId.value());
    if ( e != nullptr ) {
        simd::float3 prev = e->position();
        e->setPosition(m_deltaX/4096.f*10.f + prev.x,
                       m_deltaY/4096.f*10.f + prev.y,
                       -m_deltaZ/4096.f*10.f + prev.z);
        
        
        prev = e->rotation();
        e->setRotation(prev.x, (m_yaw * 360.f/256.f) * M_PI / 180.f);
        prev = e->cameraBoundComponent->rotation();
        e->cameraBoundComponent->setRotation((m_pitch * 360.f/256.f) * M_PI / 180.f,
                                            prev.y,
                                            prev.z);
        
        
        e->onGround = m_onGround;
    }
};

//MARK: - Update Entity Rotation
void UpdateEntityRotationPacket::read(TCPStream *stream) {
    *stream >> &m_entityId;
    *stream >> &m_yaw;
    *stream >> &m_pitch;
    *stream >> &m_onGround;
}

void UpdateEntityRotationPacket::updateGame() {
    std::shared_ptr<MobEntity> e = Engine::shared()->getEntity(m_entityId.value());
    if ( e != nullptr ) {
        simd::float3 prev = e->rotation();
        e->setRotation(prev.x, (m_yaw * 360.f/256.f) * M_PI / 180.f);
        prev = e->cameraBoundComponent->rotation();
        e->cameraBoundComponent->setRotation((m_pitch * 360.f/256.f) * M_PI / 180.f,
                                            prev.y,
                                            prev.z);
        e->onGround = m_onGround;
    }
};

//MARK: - Set Player Position And Rotation
void SetPlayerPositionAndRotation::write(TCPStream *stream) {
    MCP::VarInt packetId = MCP::VarInt(getPacketId());
    MCP::VarInt size = MCP::VarInt(packetId.size() + 3 * sizeof(double) + 2*sizeof(float) + sizeof(bool));
    
    *stream << &size;
    *stream << &packetId;
    
    *stream << &m_x;
    *stream << &m_y;
    *stream << &m_z;
    *stream << &m_yaw;
    *stream << &m_pitch;
    *stream << &m_onGround;
    
    stream->flushOutput();
};

//MARK: - Confirm Teleport
void ConfirmTeleportPacket::write(TCPStream *stream) {
    MCP::VarInt packetId(getPacketId());
    MCP::VarInt id(m_teleportId);
    MCP::VarInt size(packetId.size() + id.size());
    
    *stream << &size;
    *stream << &packetId;
    *stream << &id;
    
    stream->flushOutput();
};

//MARK: - Set Held Item
void SetHeldItemPacket::write(TCPStream *stream) {
    MCP::VarInt packetId(getPacketId());
    MCP::VarInt size(packetId.size() + sizeof(int16_t));
    
    *stream << &size;
    *stream << &packetId;
    *stream << &m_slot;
    
    stream->flushOutput();
};

//MARK: - Set Creative Mode Slot
void SetCreativeModeSlotPacket::write(TCPStream *stream) {
    MCP::VarInt packetId(getPacketId());
    MCP::VarInt itemId(m_itemId);
    
    bool present = true;
    int8_t nbt = 0x0;
    
    MCP::VarInt size(packetId.size() + // Id of packet
                     sizeof(int16_t) + // slot number
                     sizeof(bool) + // if item is present
                     itemId.size() + // Id of item
                     sizeof(int8_t) + // item count
                     sizeof(int8_t)); // NBT (for us is empty so 0x0)
    
    *stream << &size;
    *stream << &packetId;
    
    *stream << &m_slotNumber;
    *stream << &present;
    *stream << &itemId;
    *stream << &m_itemCount;
    *stream << &nbt;
    
    stream->flushOutput();
};

//MARK: - Player Action
void PlayerActionPacket::write(TCPStream *stream) {
    MCP::VarInt packetId(getPacketId());
    MCP::VarInt status(0); // Start dig status
    MCP::VarInt sequence(m_sequence);
    uint64_t position = ((m_x & 0x3FFFFFF) << 38) | ((m_z & 0x3FFFFFF) << 12) | (m_y & 0xFFF);
    int8_t face = 0;
    
    MCP::VarInt size(packetId.size() + // Id of packet
                     status.size() + // status
                     sizeof(uint64_t) + // location
                     sizeof(int8_t) + // face
                     sequence.size()); // sequence
    
    *stream << &size;
    *stream << &packetId;
    
    *stream << &status;
    *stream << &position;
    *stream << &face;
    *stream << &sequence;
    
    stream->flushOutput();
};

//MARK: - Use Item On
void UseItemOnPacket::write(TCPStream *stream) {
    MCP::VarInt packetId(getPacketId());
    MCP::VarInt hand(0);
    uint64_t position = ((m_x & 0x3FFFFFF) << 38) | ((m_z & 0x3FFFFFF) << 12) | (m_y & 0xFFF);
    MCP::VarInt face(m_face);
    float cursorX = .5f, cursorY = .5f, cursorZ = .5f;
    bool insideBlock = false;
    MCP::VarInt sequence(m_sequence);
    
    MCP::VarInt size(packetId.size() + // id
                     hand.size() + // hand
                     sizeof(uint64_t) + // location
                     face.size() + // face
                     3 * sizeof(float) + // cursor pos
                     sizeof(bool) + // inside block
                     sequence.size()); // sequence
    
    *stream << &size;
    *stream << &packetId;
    
    *stream << &hand;
    *stream << &position;
    *stream << &face;
    *stream << &cursorX;
    *stream << &cursorY;
    *stream << &cursorZ;
    *stream << &insideBlock;
    *stream << &sequence;
    
    stream ->flushOutput();
};
