//
//  PacketTypes.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 05.12.2022.
//

#ifndef PacketTypes_h
#define PacketTypes_h

namespace MCP {

//MARK: - Packet Types Enums
enum class ConnectionState: int {
    Unconnected = 0x0,
    Handshaking = 0x1,
    Playing     = 0x2
};

/// Packet types handled by both the client and the server.
/// The server distinguishes between handshaking (the first packet) and logging in (the second packet),
/// but we don't so they're both put in the handshaking category.
enum class HandshakingPacketTypes : int {
    Handshake    = 0x0, // Server bound ~~ IMPLEMENTED
    LoginStart   = 0x0, // Server bound ~~ IMPLEMENTED
    LoginSuccess = 0x2 // Client bound  ~~ IMPLEMENTED
};

/// Packet types in the playing state received by the client.
enum class ClientBoundPlayingPacketTypes: int {
    SpawnEntity                     = 0x00,
    SpawnPlayer                     = 0x02,
    AcknowledgeBlockChange          = 0x05,
    BlockDestroyStage               = 0x06,
    BlockEntityData                 = 0x07,
    BlockAction                     = 0x08,
    BlockUpdate                     = 0x09, // ~~ IMPLEMENTED
    ChangeDifficulty                = 0x0B,
    PlaySoundEffect                 = 0x17,
    Disconnect                      = 0x19,
    EntityEvent                     = 0x1A,
    UnloadChunk                     = 0x1C,
    GameEvent                       = 0x1D,
    KeepAlive                       = 0x20, // ~~ IMPLEMENTED
    ChunkData                       = 0x21, // ~~ IMPLEMENTED (mostly)
    WorldEvent                      = 0x22,
    Particle                        = 0x23,
    Login                           = 0x25,
    UpdateEntityPosition            = 0x28,
    UpdateEntityPositionAndRotation = 0x29,
    UpdateEntityRotation            = 0x2A,
    PlayerAbilities                 = 0x31,
    CombatDeath                     = 0x36,
    PlayerInfo                      = 0x37, // ~~ IMPLEMENTED
    LookAt                          = 0x38,
    SynchronizePlayerPosition       = 0x39, // ~~ IMPLEMENTED
    RemoveEntities                  = 0x3B,
    SetHeadRotation                 = 0x3F,
    UpdateSectionBlocks             = 0x40,
    SetHeldItem                     = 0x4A,
    SetCenterChunk                  = 0x4B, // ~~ IMPLEMENTED
    SetDefaultSpawnPosition         = 0x4D,
    SetEntityVelocity               = 0x52,
    SetEquipment                    = 0x53,
    SetHealth                       = 0x55,
    UpdateTime                      = 0x5C,
    EntitySoundEfect                = 0x5F,
    SoundEffect                     = 0x60,
    StopSound                       = 0x61,
    PickupItem                      = 0x65,
    TeleportEntity                  = 0x66,
    UpdateTags                      = 0x6B
};

/// Packet types in the playing state received by the server.
enum class ServerBoundPlayingPacketTypes: int {
    ConfirmTeleportation            = 0x00,
    ChangeDifficulty                = 0x02,
    ClientCommand                   = 0x07, // When the client is ready to respawn!
    ClientInformation               = 0x08,
    Interact                        = 0x19,
    KeepAlive                       = 0x12, // ~~ IMPLEMENTED
    SetPlayerPosition               = 0x14,
    SetPlayerPositionAndRotation    = 0x15,
    SetPlayerRotation               = 0x16,
    SetPlayerOnGround               = 0x17,
    PickItem                        = 0x1A,
    PlayerAction                    = 0x1D,
    PlayerCommand                   = 0x1E,
    PlayerInput                     = 0x1F,
    SetHeldItem                     = 0x28,
    SetCreativeModeSlot             = 0x2B,
    TeleportToEntity                = 0x30,
    UseItemOn                       = 0x31,
    UseItem                         = 0x32
};

}

#endif /* PacketTypes_h */
