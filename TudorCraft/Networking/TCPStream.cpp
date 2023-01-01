//
//  TCPStream.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 02.12.2022.
//

#include "TCPStream.hpp"

#include "World.hpp"
#include "Chunk.hpp"

//TODO: Create utlity methods to check for overflow

/*
 Explicit instantiation for templated operator.
 
 Not doing this results in a linker error because the linker doesn't know
 what functions to create from the template when it compiles TCPStream.o.
 As such, when it gets to compiling Packet.o (for example) it won't find
 some operators like the ones below.
 */
template const TCPStream &TCPStream::operator<<(short *value);
template const TCPStream &TCPStream::operator>>(short *value);
template const TCPStream &TCPStream::operator>>(double *value);
template const TCPStream &TCPStream::operator>>(float *value);
template const TCPStream &TCPStream::operator>>(int *value);
template const TCPStream &TCPStream::operator>>(uint8_t *value);

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

void TCPStream::skipRestOfPacket() {
    int64_t lengthToSkip = m_currentPacketLength - m_currentPacketBytesRead;
    
    readBytes(m_recvBuffer, lengthToSkip);
    
    flushInput();
};

size_t TCPStream::readBytes(uint8_t *buffer, size_t length) {
    int64_t read = 0;
    
    while ( length > 0 ) {
        int64_t biggestSizePossible = (length > RECV_BUFFER_SIZE) ? RECV_BUFFER_SIZE : length;
        int64_t bytesRead = recv(m_socketDescriptor, buffer+read, biggestSizePossible, 0);
        
        m_currentPacketBytesRead += bytesRead;
        length -= bytesRead;
        read += bytesRead;
        
        if ( bytesRead == 0 || bytesRead == -1 ) {
            *m_connectionState = MCP::ConnectionState::Unconnected;
            return -1;
        }
    }
    
    return read;
};

#warning Refactor this to return the actual amount of sent bytes.
size_t TCPStream::writeBytes(uint8_t *buffer, size_t length) {
    memcpy(m_sendBuffer+m_sendBufferCurrentOffset, buffer, length);
    m_sendBufferCurrentOffset += length;
    
    flushOutput();
    
    return length;
};

//MARK: - Primitives template
template<class T>
const TCPStream &TCPStream::operator>>(T *value) {
    unsigned char *toReceive = reinterpret_cast<unsigned char *>(value);
    readBytes(toReceive, sizeof(T));
    std::reverse(toReceive, toReceive+sizeof(T));
    
    return *this;
};

template<class T>
const TCPStream &TCPStream::operator<<(T *value) {
    unsigned char *toSend = reinterpret_cast<unsigned char *>(value);
    std::reverse(toSend, toSend+sizeof(T));
    
    memcpy(m_sendBuffer+m_sendBufferCurrentOffset, toSend, sizeof(T));
    m_sendBufferCurrentOffset += sizeof(T);
    
    return *this;
};

//MARK: - MCP::Packet template specialization
template<>
const TCPStream &TCPStream::operator>>(MCP::Packet *packet) {
    MCP::VarInt packetLength;
    MCP::VarInt packetId;
    
    *this >> &packetLength;
    
    m_currentPacketLength = packetLength.value();
    m_currentPacketBytesRead = 0;
    
    *this >> &packetId;
    
//    printf("Received packed with id: 0x%x\n", packetId.value());
//    printf("Length: %d\n", packetLength.value());
    
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
                
            case (int) MCP::ClientBoundPlayingPacketTypes::ChunkData: {
                std::cout << "ChunkData packet" << std::endl;
                
                MCP::ChunkDataPacket *chunkDataPacket = new MCP::ChunkDataPacket;
                chunkDataPacket->read(this);
                packet = chunkDataPacket;
                
                chunkDataPacket->updateGame();
                
                break;
            }
                
            default: {
                // We don't support this packet yet, so skip the length of the buffer
                int64_t lengthToSkip = packetLength.value() - packetId.size();
                readBytes(m_recvBuffer, lengthToSkip);
                flushInput();
                
                break;
            }
        }
    }
    
//    std::cout << std::endl;
    
    return *this;
};

template<>
const TCPStream &TCPStream::operator<<(MCP::Packet *packet) {
    packet->write(this);
    
    return *this;
};

//MARK: - MCP::Property template specialization
template<>
const TCPStream &TCPStream::operator>>(MCP::Property *property) {
    *this >> &property->name;
    *this >> &property->value;
    *this >> (int8_t*) &property->isSigned;
    if ( property->isSigned ) {
        *this >> &property->signature;
    }
    
    return *this;
};

template<>
const TCPStream &TCPStream::operator<<(MCP::Property *property) {
    *this << &property->name;
    *this << &property->value;
    *this << &property->isSigned;
    if ( property->isSigned ) {
        *this << &property->signature;
    }
    
    return *this;
};

//MARK: - MCP::VarInt template specialization
template<>
const TCPStream &TCPStream::operator>>(MCP::VarInt *value) {
    int position = 0;
    int readValue = 0;
    unsigned char currentByte = 0;
    
    while ( true ) {
        readBytes(&currentByte, 1); // Read current byte
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

template<>
const TCPStream &TCPStream::operator<<(MCP::VarInt *value) {
    memcpy(m_sendBuffer + m_sendBufferCurrentOffset, value->buffer(), value->size());
    m_sendBufferCurrentOffset += value->size();
    
    return *this;
};

//MARK: - string template specialization
template<>
const TCPStream &TCPStream::operator>>(std::string *value) {
    MCP::VarInt length;
    *this >> &length;
    

    readBytes(m_recvBuffer, length.value());
    // Is the string deallocator called here for the previous value???
    *value = std::string((char *)m_recvBuffer, length.value());
    flushInput();
    
    return *this;
};

template<>
const TCPStream &TCPStream::operator<<(std::string *value) {
    MCP::VarInt strLength((int)value->size());
    *this << &strLength;
    
    memcpy(m_sendBuffer + m_sendBufferCurrentOffset, value->data(), value->size());
    m_sendBufferCurrentOffset += value->size();
    
    return *this;
};

//MARK: - MCP::Uuid template specialization
template<>
const TCPStream &TCPStream::operator>>(MCP::Uuid *value) {
    *this >> &(value->mostSignificant);
    *this >> &(value->leastSignificat);
    
    return *this;
};

template<>
const TCPStream &TCPStream::operator<<(MCP::Uuid *value) {
    *this << &value->mostSignificant;
    *this << &value->leastSignificat;
    
    return *this;
};

//MARK: - MCP::NBT::Tag template specialization
template<>
const TCPStream &TCPStream::operator>>(MCP::NBT::Tag *value) {
    value->decodeFrom(this);
    
    return *this;
};

template<>
const TCPStream &TCPStream::operator<<(MCP::NBT::Tag *value) {
    value->encodeTo(this);
    
    return *this;
};
