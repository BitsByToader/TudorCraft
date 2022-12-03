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
    *stream << m_serverAddress;
    *stream << m_port;
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
    
    *stream << &size;
    *stream << &packetId;
    *stream << m_username;
    *stream << false;
    *stream << false;
    
    stream->flushOutput();
};

//MARK: - Login Success Packet
void LoginSuccessPacket::read(TCPStream *stream) {
    MCP::VarInt size;
    MCP::VarInt packetId;
    MCP::VarInt propertiesCount;
    
    *stream >> &size;
    std::cout << size.value() << std::endl;
    
    *stream >> &packetId;
    std::cout << packetId.value() << std::endl;
    
    *stream >> &m_uuid;
    std::cout << m_uuid.mostSignificant << " " << m_uuid.leastSignificat << std::endl;
    
    *stream >> m_username;
    std::cout << m_username << std::endl;
    
    *stream >> &propertiesCount;
    std::cout << propertiesCount.value() << std::endl;
    
//    std::cout << size.value() << packetId.value() << name << uuid.mostSignificant << uuid.leastSignificat << propertiesCount.value() << std::endl;
};


