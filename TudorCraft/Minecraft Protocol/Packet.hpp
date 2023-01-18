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
#include <memory>

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

//MARK: - Login Start
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

//MARK: - Login Success
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

//MARK: - Player Info
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

//MARK: - Keep Alive
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

//MARK: - Synchronize Player Position
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

//MARK: - Center Chunk
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

//MARK: - Chunk Data
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

//MARK: - Block Update
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

//MARK: - Spawn Player
class SpawnPlayerPacket: public Packet {
public:
    ~SpawnPlayerPacket() override = default;
    
    void write(TCPStream *stream) override {};
    void read(TCPStream *stream) override;
    
    int getPacketId() override {
        return (int) MCP::ClientBoundPlayingPacketTypes::SpawnPlayer;
    }
    
    void updateGame();
    
private:
    MCP::VarInt m_entityId;
    MCP::Uuid m_playerUuid;
    double m_x, m_y, m_z;
    uint8_t m_yaw;
    uint8_t m_pitch;
};

//MARK: - Spawn Entity
class SpawnEntityPacket: public Packet {
public:
    ~SpawnEntityPacket() override = default;
    
    void write(TCPStream *stream) override {};
    void read(TCPStream *stream) override;
    
    int getPacketId() override {
        return (int) MCP::ClientBoundPlayingPacketTypes::SpawnEntity;
    }
    
    void updateGame();
    
private:
    MCP::VarInt m_entityId;
    MCP::Uuid m_entityUuid;
    MCP::VarInt m_type;
    double m_x, m_y, m_z;
    uint8_t m_yaw;
    uint8_t m_pitch;
    uint8_t m_headYaw;
    MCP::VarInt m_data;
    int16_t m_velocityX;
    int16_t m_velocityY;
    int16_t m_velocityZ;
};

//MARK: - Remove Entities
class RemoveEntitiesPacket: public Packet {
public:
    ~RemoveEntitiesPacket() override = default;
    
    void write(TCPStream *stream) override {};
    void read(TCPStream *stream) override;
    
    int getPacketId() override {
        return (int) MCP::ClientBoundPlayingPacketTypes::RemoveEntities;
    }
    
    void updateGame();
    
private:
    MCP::VarInt m_count;
    std::vector<MCP::VarInt> m_ids;
};

//MARK: - Set Entity Velocity
class SetEntityVelocityPacket: public Packet {
public:
    ~SetEntityVelocityPacket() override = default;
    
    void write(TCPStream *stream) override {};
    void read(TCPStream *stream) override;
    
    int getPacketId() override {
        return (int) MCP::ClientBoundPlayingPacketTypes::SetEntityVelocity;
    }
    
    void updateGame();
    
private:
    MCP::VarInt m_entityId;
    int16_t m_velocityX, m_velocityY, m_velocityZ;
};

//MARK: - Update Entity Position
class UpdateEntityPositionPacket: public Packet {
public:
    ~UpdateEntityPositionPacket() override = default;
    
    void write(TCPStream *stream) override {};
    void read(TCPStream *stream) override;
    
    int getPacketId() override {
        return (int) MCP::ClientBoundPlayingPacketTypes::UpdateEntityPosition;
    }
    
    void updateGame();
    
private:
    MCP::VarInt m_entityId;
    int16_t m_deltaX, m_deltaY, m_deltaZ;
    uint8_t m_onGround;
};

//MARK: - Update Entity Position And Rotation
class UpdateEntityPositionAndRotationPacket: public Packet {
public:
    ~UpdateEntityPositionAndRotationPacket() override = default;
    
    void write(TCPStream *stream) override {};
    void read(TCPStream *stream) override;
    
    int getPacketId() override {
        return (int) MCP::ClientBoundPlayingPacketTypes::UpdateEntityPositionAndRotation;
    }
    
    void updateGame();
    
private:
    MCP::VarInt m_entityId;
    int16_t m_deltaX, m_deltaY, m_deltaZ;
    uint8_t m_yaw, m_pitch;
    uint8_t m_onGround;
};

//MARK: - Update Entity Rotation
class UpdateEntityRotationPacket: public Packet {
public:
    ~UpdateEntityRotationPacket() override = default;
    
    void write(TCPStream *stream) override {};
    void read(TCPStream *stream) override;
    
    int getPacketId() override {
        return (int) MCP::ClientBoundPlayingPacketTypes::UpdateEntityRotation;
    }
    
    void updateGame();
    
private:
    MCP::VarInt m_entityId;
    uint8_t m_yaw, m_pitch;
    uint8_t m_onGround;
};

//MARK: - Set Player Position
class SetPlayerPositionAndRotation: public Packet {
public:
    ~SetPlayerPositionAndRotation() override = default;
    
    void write(TCPStream *stream) override;
    void read(TCPStream *stream) override {};
    
    int getPacketId() override {
        return (int) MCP::ServerBoundPlayingPacketTypes::SetPlayerPositionAndRotation;
    }
    
    double m_x, m_y, m_z;
    float m_pitch, m_yaw;
    bool m_onGround;
};

//MARK: - Confirm Teleport
class ConfirmTeleportPacket: public Packet {
public:
    ConfirmTeleportPacket(int id) {
        m_teleportId = MCP::VarInt(id);
    };
    
    ~ConfirmTeleportPacket() override = default;
    
    void write(TCPStream *stream) override;
    void read(TCPStream *stream) override {};
    
    int getPacketId() override {
        return (int) MCP::ServerBoundPlayingPacketTypes::ConfirmTeleportation;
    }
    
private:
    MCP::VarInt m_teleportId;
};

//MARK: - Set Held Item
class SetHeldItemPacket: public Packet {
public:
    SetHeldItemPacket(int16_t slot) {
        m_slot = slot;
    }
    ~SetHeldItemPacket() override = default;
    
    void write(TCPStream *stream) override;
    void read(TCPStream *stream) override {};
    
    int getPacketId() override {
        return (int) MCP::ServerBoundPlayingPacketTypes::SetHeldItem;
    }
    
private:
    int16_t m_slot;
};

//MARK: - Set Creative Mode Slot
class SetCreativeModeSlotPacket: public Packet {
public:
    SetCreativeModeSlotPacket(int slotNumber, int itemId) {
        // Currently only support using one slot
        m_slotNumber = 36 + slotNumber;
        m_itemId = itemId;
        m_itemCount = 1;
    };
    ~SetCreativeModeSlotPacket() override = default;
    
    void write(TCPStream *stream) override;
    void read(TCPStream *stream) override {};
    
    int getPacketId() override {
        return (int) MCP::ServerBoundPlayingPacketTypes::SetCreativeModeSlot;
    }
    
private:
    int16_t m_slotNumber;
    int m_itemId;
    int8_t m_itemCount;
    // The packet also has a NBT member for item detail (enchatements, etc), but we don't use it currently.
};

//MARK: - Player Action
class PlayerActionPacket: public Packet {
public:
    PlayerActionPacket(int x, int y, int z, int sequence) {
        m_x = x;
        m_y = y;
        m_z = z;
        m_sequence = sequence;
    };
    ~PlayerActionPacket() override = default;
    
    void write(TCPStream *stream) override;
    void read(TCPStream *stream) override {};
    
    int getPacketId() override {
        return (int) MCP::ServerBoundPlayingPacketTypes::PlayerAction;
    }
    
private:
    uint64_t m_x, m_y, m_z;
    int m_sequence;
};

// MARK: - Use Item On
class UseItemOnPacket: public Packet {
public:
    UseItemOnPacket(int x, int y, int z, int face, int sequence) {
        m_x = x;
        m_y = y;
        m_z = z;
        m_face = face;
        m_sequence = sequence;
    };
    
    ~UseItemOnPacket() override = default;
    
    void write(TCPStream *stream) override;
    void read(TCPStream *stream) override {};
    
    int getPacketId() override {
        return (int) MCP::ServerBoundPlayingPacketTypes::UseItemOn;
    }
    
private:
    uint64_t m_x, m_y, m_z;
    int m_face;
    int m_sequence;
};

// MARK: -

}
#endif /* Packet_hpp */
