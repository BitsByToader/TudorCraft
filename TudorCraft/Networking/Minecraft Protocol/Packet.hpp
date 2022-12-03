//
//  Packet.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 02.12.2022.
//

#ifndef Packet_hpp
#define Packet_hpp

// Std Library
#include <iostream>
#include <string>

#include "VarInt.hpp"
#include "Uuid.hpp"

class TCPStream;

namespace MCP {

//MARK: - Packet Types Enums
enum class HandshakingPacketTypes : unsigned int {
    Handshake = 0x0,
    LoginStart = 0x0,
    LoginSuccess = 0x2
};

//MARK: - Base Packet Class
class Packet {
public:
    virtual void write(TCPStream *stream) = 0;
    virtual void read(TCPStream *stream) = 0;
    
    virtual unsigned int getPacketId() = 0;
//    virtual unsigned int getSize() = 0;
};

//MARK: - Hanshake Packet
class HandshakePacket: public Packet {
public:
    HandshakePacket(int protocolVersion, std::string m_serverAddress, short port);
    
    void write(TCPStream *stream) override;
    void read(TCPStream *stream) override {};
    
    unsigned int getPacketId() override {
        return (unsigned int)HandshakingPacketTypes::Handshake;
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
    
    void write(TCPStream *stream) override;
    void read(TCPStream *stream) override {};
    
    unsigned int getPacketId() override {
        return (unsigned int) HandshakingPacketTypes::LoginStart;
    };
    
private:
    std::string m_username;
};

//MARK: - Login Success Packet
class LoginSuccessPacket: public Packet {
public:
//    LoginSuccessPacket();
    
    void write(TCPStream *stream) override {};
    void read(TCPStream *stream) override;
    
    unsigned int getPacketId() override {
        return (unsigned int) HandshakingPacketTypes::LoginSuccess;
    }
    
private:
    MCP::Uuid m_uuid;
    std::string m_username;
};

}

#endif /* Packet_hpp */
