//
//  Packet.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 02.12.2022.
//

#ifndef Packet_hpp
#define Packet_hpp

// Std Library
#include <string>
#include <vector>
#include <array>

#include "PacketTypes.hpp"
#include "VarInt.hpp"
#include "Uuid.hpp"
#include "Nbt.hpp"

class TCPStream;

namespace MCP {

//MARK: - Property struct
struct Property {
    std::string name;
    std::string value;
    bool isSigned;
    std::string signature;
};

//MARK: - Base Packet Class
class Packet {
public:
    virtual ~Packet() {};
    
    virtual void write(TCPStream *stream) = 0;
    virtual void read(TCPStream *stream) = 0;
    
    virtual int getPacketId() = 0;
//    virtual unsigned int getSize() = 0;
};

//MARK: - Hanshake Packet
class HandshakePacket: public Packet {
public:
    HandshakePacket(int protocolVersion, std::string m_serverAddress, short port);
    ~HandshakePacket() override = default;
    
    void write(TCPStream *stream) override;
    void read(TCPStream *stream) override {};
    
    int getPacketId() override {
        return (int)HandshakingPacketTypes::Handshake;
    };
    
private:
    MCP::VarInt m_protocolVersion;
    std::string m_serverAddress;
    short m_port;
    // We don't care about getting the status, so always go to login.
    MCP::VarInt nextState = MCP::VarInt(2);
};

//MARK: - Login Start Packet
class LoginStartPacket: public Packet {
public:
    LoginStartPacket(std::string username);
    ~LoginStartPacket() override = default;
    
    void write(TCPStream *stream) override;
    void read(TCPStream *stream) override {};
    
    int getPacketId() override {
        return (int) HandshakingPacketTypes::LoginStart;
    };
    
private:
    std::string m_username;
};

//MARK: - Login Success Packet
class LoginSuccessPacket: public Packet {
public:
//    LoginSuccessPacket();
    ~LoginSuccessPacket() override = default;
    
    void write(TCPStream *stream) override {};
    void read(TCPStream *stream) override;
    
    int getPacketId() override {
        return (int) HandshakingPacketTypes::LoginSuccess;
    }
    
private:
    MCP::Uuid m_uuid;
    std::string m_username;
    std::vector<Property> m_properties;
};

//MARK: - Player Info Packet
class PlayerInfoPacket: public Packet {
public:
    ~PlayerInfoPacket() override = default;
    
    void write(TCPStream *stream) override {};
    void read(TCPStream *stream) override;
    
    int getPacketId() override {
        return (int) MCP::ClientBoundPlayingPacketTypes::PlayerInfo;
    }
    
    struct Player {
        std::string name;
        MCP::VarInt propertiesCount;
        std::vector<MCP::Property> properties;
        MCP::VarInt gamemode;
        MCP::VarInt ping;
        bool hasDisplayName;
        std::string displayName;
        bool hasSigData;
        int64_t timestamp;
        MCP::VarInt keyLength;
        std::vector<int8_t> key;
        MCP::VarInt signatureLength;
        std::vector<int8_t> signature;
    };
    
    struct NewDisplayName {
        bool hasDisplayName;
        std::string displayName;
    };
    
private:
    //TODO: Convert these to std::optional<T>
    MCP::VarInt m_action;
    std::vector<MCP::Uuid> m_uuids;
    std::vector<Player> m_players;
    std::vector<MCP::VarInt> m_gamemodes;
    std::vector<MCP::VarInt> m_pings;
    std::vector<NewDisplayName> m_names;
};

//MARK: - Keep Alive Packet
class KeepAlivePacket: public Packet {
public:
    ~KeepAlivePacket() override = default;
    
    void write(TCPStream *stream) override;
    void read(TCPStream *stream) override;
    
    int getPacketId() override {
        return (int) MCP::ClientBoundPlayingPacketTypes::KeepAlive;
    }
    
private:
    int64_t m_id;
};

//MARK: - Synchronize Player Position Packet
class SynchronizePlayerPositionPacket: public Packet {
public:
    ~SynchronizePlayerPositionPacket() override = default;
    
    void write(TCPStream *stream) override {};
    void read(TCPStream *stream) override;
    
    int getPacketId() override {
        return (int) MCP::ClientBoundPlayingPacketTypes::SynchronizePlayerPosition;
    }
    
private:
    double m_x;
    double m_y;
    double m_z;
    float m_pitch;
    float m_yaw;
    uint8_t m_flags;
    MCP::VarInt m_teleportId;
    bool m_dismountVehicle;
};

//MARK: - Center Chunk Packet
class CenterChunkPacket: public Packet {
public:
    ~CenterChunkPacket() override = default;
    
    void write(TCPStream *stream) override {};
    void read(TCPStream *stream) override;
    
    int getPacketId() override {
        return (int) MCP::ClientBoundPlayingPacketTypes::SetCenterChunk;
    }
    
private:
    MCP::VarInt m_chunkX;
    MCP::VarInt m_chunkZ;
};

//MARK: - Chunk Data Packet
class ChunkDataPacket: public Packet {
public:
    ~ChunkDataPacket() override = default;

    void write(TCPStream *stream) override {};
    void read(TCPStream *stream) override;

    int getPacketId() override {
        return (int) MCP::ClientBoundPlayingPacketTypes::ChunkData;
    }
    
    void updateGame();
    
    struct ChunkSection {
        struct PalletedContainer {
            uint8_t bitsPerEntry;
            VarInt palleteLength;
            VarInt palleteValue; // if bitsPerEntry == 0
            std::vector<VarInt> pallete;
            VarInt dataArrayLength;
            std::vector<int64_t> dataArray;
        };
        
        int16_t blockCount;
        PalletedContainer blockStates;
        PalletedContainer biomes;
    };
    
private:
    int m_chunkX;
    int m_chunkZ;
    NBT::Tag m_heightmap;
    VarInt m_dataSize;
    std::vector<ChunkSection> m_data;
};

class BlockUpdatePacket: public Packet {
public:
    ~BlockUpdatePacket() override = default;

    void write(TCPStream *stream) override {};
    void read(TCPStream *stream) override;

    int getPacketId() override {
        return (int) MCP::ClientBoundPlayingPacketTypes::BlockUpdate;
    }
    
    void updateGame();
private:
    uint64_t m_blockPosition;
    MCP::VarInt m_newState;
};

}

#endif /* Packet_hpp */
