//
//  TCPStream.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 02.12.2022.
//

#include "TCPStream.hpp"

//TODO: Create utlity methods to check for overflow
//TODO: VarInts are currently being read from the network byte by byte, which is inherrently slow(er),
//      so test how slow this is and optionally read the whole 5 bytes, and if there isn't a need for that         many, put them in m_recvBuffer and take them from there for the next read.

TCPStream::TCPStream(MCP::ConnectionState *state, std::string serverAddress, int port) {
    m_connectionState = state;
    
    memset(m_sendBuffer, 0x0, RECV_BUFFER_SIZE);
    memset(m_sendBuffer, 0x0, SEND_BUFFER_SIZE);
    
    // Configure the TCP socket
    m_socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if ( m_socketDescriptor == -1 ) {
        AAPL_PRINT("Couldn't create TCP socket!");
        return;
    }
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if ( inet_pton(AF_INET, serverAddress.c_str(), &addr.sin_addr) <= 0 ) {
        AAPL_PRINT("Server address is invalid!");
        return;
    }
    
    if ( connect(m_socketDescriptor, (struct sockaddr *)&addr, sizeof(addr)) < 0 ) {
        AAPL_PRINT("Couldn't connect to server!");
        return;
    }
    
    AAPL_PRINT("Connected to TCP server!");
};

TCPStream::~TCPStream() {
    AAPL_PRINT("TCPStream Done.");
    close(m_socketDescriptor);
};

void TCPStream::flushOutput() {
    send(m_socketDescriptor, m_sendBuffer, m_sendBufferCurrentOffset, 0);
    
    memset(m_sendBuffer, 0x0, SEND_BUFFER_SIZE);
    m_sendBufferCurrentOffset = 0;
};

void TCPStream::flushInput() {
    memset(m_recvBuffer, 0x0, RECV_BUFFER_SIZE);
    m_recvBufferCurrentOffset = 0;
};

long TCPStream::receiveFromSocket(void *buffer, int64_t length) {
    long bytesRead = recv(m_socketDescriptor, buffer, length, 0);
    
    if ( bytesRead == 0 || bytesRead == -1 ) {
        *m_connectionState = MCP::ConnectionState::Unconnected;
        return -1;
    }
    
    return bytesRead;
};

const TCPStream &TCPStream::operator>>(MCP::Packet *packet) {
    MCP::VarInt packetLength;
    MCP::VarInt packetId;
    
    *this >> &packetLength;
    *this >> &packetId;
    
    printf("Received packed with id: 0x%x\n", packetId.value());
    printf("Length: %d\n", packetLength.value());
    
    if ( *m_connectionState == MCP::ConnectionState::Handshaking ) {
        if ( packetId.value() == (int)MCP::HandshakingPacketTypes::LoginSuccess ) {
            std::cout << "LoginSuccess packet" << std::endl;
            
            MCP::LoginSuccessPacket *loginSuccessPacket = new MCP::LoginSuccessPacket;
            loginSuccessPacket->read(this);
            *m_connectionState = MCP::ConnectionState::Playing;
            packet = loginSuccessPacket;
        }
        
    } else if ( *m_connectionState == MCP::ConnectionState::Playing ) {
        switch (packetId.value()) {
            case (int) MCP::ClientBoundPlayingPacketTypes::PlayerInfo: {
                std::cout << "PlayerInfo packet" << std::endl;
                
                MCP::PlayerInfoPacket *playerInfoPacket = new MCP::PlayerInfoPacket;
                playerInfoPacket->read(this);
                packet = playerInfoPacket;
                
                break;
            }
                
            case (int) MCP::ClientBoundPlayingPacketTypes::KeepAlive: {
                std::cout << "KeepAlive packet" << std::endl;
                
                MCP::KeepAlivePacket *keepAlivePacket = new MCP::KeepAlivePacket;
                keepAlivePacket->read(this);
                keepAlivePacket->write(this);
                packet = keepAlivePacket;
                
                break;
            }
                
            case (int) MCP::ClientBoundPlayingPacketTypes::SynchronizePlayerPosition: {
                std::cout << "Synchronize player position packet" << std::endl;
                
                MCP::SynchronizePlayerPositionPacket *sppPacket = new MCP::SynchronizePlayerPositionPacket;
                sppPacket->read(this);
                packet = sppPacket;
                
                break;
            }
                
            case (int) MCP::ClientBoundPlayingPacketTypes::SetCenterChunk: {
                std::cout << "SetCenterChunk packet" << std::endl;
                
                MCP::CenterChunkPacket *centerChunkPacket = new MCP::CenterChunkPacket;
                centerChunkPacket->read(this);
                packet = centerChunkPacket;
                
                break;
            }
                
            default: {
                int64_t lengthToSkip = packetLength.value() - packetId.size();
                // We don't support this packet yet, so skip the length of the buffer
                
                while ( lengthToSkip > 0 ) {
                    int64_t biggestSizePossible = (lengthToSkip - RECV_BUFFER_SIZE > 0) ? RECV_BUFFER_SIZE : lengthToSkip;
                    
                    int64_t bytesRead = receiveFromSocket(m_recvBuffer, biggestSizePossible);
                    
                    lengthToSkip -= bytesRead;
                    
                    if ( bytesRead <= 0 ) {
                        break;
                    }
                }
                
                break;
            }
        }
    }
    
    std::cout << std::endl;
    
    return *this;
};

const TCPStream &TCPStream::operator<<(MCP::Packet *packet) {
    packet->write(this);
    
    return *this;
};

const TCPStream &TCPStream::operator>>(MCP::Property *property) {
    *this >> property->name;
    *this >> property->value;
    *this >> (int8_t*) &property->isSigned;
    if ( property->isSigned ) {
        *this >> property->signature;
    }
    
    return *this;
};

const TCPStream &TCPStream::operator<<(MCP::Property *property) {
    *this << property->name;
    *this << property->value;
    *this << property->isSigned;
    if ( property->isSigned ) {
        *this << property->signature;
    }
    
    return *this;
};

const TCPStream &TCPStream::operator>>(MCP::VarInt *value) {
    int position = 0;
    int readValue = 0;
    unsigned char currentByte = 0;
    
    while ( true ) {
        receiveFromSocket(&currentByte, 1); // Read current byte
        readValue |= ( currentByte & SEGMENT_BITS ) << position*7;
        
        if ( (currentByte & CONTINUE_BIT) == 0 ) {
            break;
        }
        
        ++position;
        
        if ( position*7 >= 32 ) {
            AAPL_PRINT("VarInt is too big.");
            break;
        }
    }
    
    *value = MCP::VarInt(readValue);
    
//    m_size = position+1;
    
    return *this;
};
#warning "Question: Pointer vs Reference?"
const TCPStream &TCPStream::operator<<(MCP::VarInt *value) {
    memcpy(m_sendBuffer + m_sendBufferCurrentOffset, value->buffer(), value->size());
    m_sendBufferCurrentOffset += value->size();
    
    return *this;
};

const TCPStream &TCPStream::operator>>(std::string &value) {
    MCP::VarInt length;
    *this >> &length;
    

    receiveFromSocket(m_recvBuffer, length.value());
    // Is the string deallocator called here for the previous value???
#warning "THIS NEEDS A REWRITE PRONTO"
    value = std::string((char *)m_recvBuffer, length.value());
    flushInput();
    
    return *this;
};

const TCPStream &TCPStream::operator<<(std::string &value) {
    MCP::VarInt strLength((int)value.size());
    *this << &strLength;
    
    memcpy(m_sendBuffer + m_sendBufferCurrentOffset, value.data(), value.size());
    m_sendBufferCurrentOffset += value.size();
    
    return *this;
};

const TCPStream &TCPStream::operator>>(MCP::Uuid *value) {
    *this >> &(value->mostSignificant);
    *this >> &(value->leastSignificat);
    
    return *this;
};

const TCPStream &TCPStream::operator<<(MCP::Uuid value) {
    *this << value.mostSignificant;
    *this << value.leastSignificat;
    
    return *this;
};

const TCPStream &TCPStream::operator>>(int64_t *value) {
    unsigned char *toReceive = reinterpret_cast<unsigned char *>(value);
    receiveFromSocket(toReceive, sizeof(int64_t));
    std::reverse(toReceive, toReceive+sizeof(int64_t));
    
    return *this;
};

const TCPStream &TCPStream::operator<<(int64_t value) {
    // Network order is big-endian, which happens to coincide with the Minecraft Protocol.
    unsigned char *toSend = reinterpret_cast<unsigned char *>(&value);
    std::reverse(toSend, toSend+sizeof(int64_t));
    
    memcpy(m_sendBuffer+m_sendBufferCurrentOffset, toSend, sizeof(int64_t));
    m_sendBufferCurrentOffset += sizeof(int64_t);
    
    return *this;
};

const TCPStream &TCPStream::operator>>(int32_t *value) {
    unsigned char *toReceive = reinterpret_cast<unsigned char *>(value);
    receiveFromSocket(toReceive, sizeof(int32_t));
    
    std::reverse(toReceive, toReceive+sizeof(int32_t));
    
    return *this;
};

const TCPStream &TCPStream::operator<<(int32_t value) {
    // Network order is big-endian, which happens to coincide with the Minecraft Protocol.
    unsigned char *toSend = reinterpret_cast<unsigned char *>(&value);
    std::reverse(toSend, toSend+sizeof(int32_t));
    
    memcpy(m_sendBuffer+m_sendBufferCurrentOffset, toSend, sizeof(int32_t));
    m_sendBufferCurrentOffset += sizeof(int32_t);
    
    return *this;
};

const TCPStream &TCPStream::operator>>(int16_t *value) {
    unsigned char *toReceive = reinterpret_cast<unsigned char *>(value);
    receiveFromSocket(toReceive, sizeof(int16_t));
    
    std::reverse(toReceive, toReceive+sizeof(int16_t));
    
    return  *this;
};

const TCPStream &TCPStream::operator<<(int16_t value) {
    unsigned char *toSend = reinterpret_cast<unsigned char *>(&value);
    std::reverse(toSend, toSend+sizeof(int16_t));
    
    memcpy(m_sendBuffer+m_sendBufferCurrentOffset, toSend, sizeof(int16_t));
    m_sendBufferCurrentOffset += sizeof(int16_t);
    
    return *this;
};

const TCPStream &TCPStream::operator>>(int8_t *value) {
    receiveFromSocket(value, 1);
    
    return *this;
};

const TCPStream &TCPStream::operator<<(int8_t value) {
    m_sendBuffer[m_sendBufferCurrentOffset] = (char) value;
    ++m_sendBufferCurrentOffset;
    
    return *this;
};

const TCPStream &TCPStream::operator>>(double *value) {
    unsigned char *toReceive = reinterpret_cast<unsigned char *>(value);
    receiveFromSocket(toReceive, sizeof(double));
    
    std::reverse(toReceive, toReceive+sizeof(double));
    
    return  *this;
};

const TCPStream &TCPStream::operator<<(double value) {
    unsigned char *toSend = reinterpret_cast<unsigned char *>(&value);
    std::reverse(toSend, toSend+sizeof(double));
    
    memcpy(m_sendBuffer+m_sendBufferCurrentOffset, toSend, sizeof(double));
    m_sendBufferCurrentOffset += sizeof(double);
    
    return *this;
};

const TCPStream &TCPStream::operator>>(float *value) {
    unsigned char *toReceive = reinterpret_cast<unsigned char *>(value);
    receiveFromSocket(toReceive, sizeof(float));
    
    std::reverse(toReceive, toReceive+sizeof(float));
    
    return  *this;
};

const TCPStream &TCPStream::operator<<(float value) {
    unsigned char *toSend = reinterpret_cast<unsigned char *>(&value);
    std::reverse(toSend, toSend+sizeof(float));
    
    memcpy(m_sendBuffer+m_sendBufferCurrentOffset, toSend, sizeof(float));
    m_sendBufferCurrentOffset += sizeof(float);
    
    return *this;
};
