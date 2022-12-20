//
//  Packet.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 02.12.2022.
//

#include "Packet.hpp"

#include "TCPStream.hpp"

using namespace MCP;

//MARK: - Handshake Packet
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

//MARK: - Login Start Packet
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

//MARK: - Login Success Packet
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
};

//MARK: - Player Info Packet
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

//MARK: - Keep Alive Packet
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

//MARK: - Synchronize Player Position Packet
void SynchronizePlayerPositionPacket::read(TCPStream *stream) {
    *stream >> &m_x;
    *stream >> &m_y;
    *stream >> &m_z;
    
    *stream >> &m_yaw;
    *stream >> &m_pitch;
    
    *stream >> (int8_t *) &m_flags;
    *stream >> &m_teleportId;
    *stream >> (int8_t *) &m_dismountVehicle;
};

//MARK: - Center Chunk Packet
void CenterChunkPacket::read(TCPStream *stream) {
    *stream >> &m_chunkX;
    *stream >> &m_chunkZ;
};

//MARK: - Chunk Data Packet
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
        *stream >> &section.blockStates.palleteLength;
        
        section.blockStates.pallete.reserve(section.blockStates.palleteLength.value());
        for ( int i = 0; i < section.blockStates.palleteLength.value(); i++ ) {
            VarInt v;
            *stream >> &v;
            section.blockStates.pallete.push_back(v);
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
        *stream >> &section.biomes.palleteLength;
        
        section.biomes.pallete.reserve(section.biomes.palleteLength.value());
        for ( int i = 0; i < section.biomes.palleteLength.value(); i++ ) {
            VarInt v;
            *stream >> &v;
            section.biomes.pallete.push_back(v);
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
};
