//
//  TCPStream.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 02.12.2022.
//

#include "TCPStream.hpp"

#warning "TODOS"
//TODO: Use templates to generalize the >> operators for primitives like int, short, etc
//TODO: Create a setter for `m_sendBufferCurrentOffset` to check if we go over the max buffer size
//TODO: VarInts are currently being read from the network byte by byte, which is inherrently pretty slow,
//      so test how slow this is and optionally read the whole 5 bytes, and if there isn't a need for that many,
//      put them in m_recvBuffer and take them from there for the next read

//TODO: Currently

TCPStream::TCPStream(std::string serverAddress, int port) {
    flushInput();
//    flushOutput();
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
    
//    mythread = std::thread(&TCPStream::test, this);
    
    AAPL_PRINT("Connected to TCP server!");
};

TCPStream::~TCPStream() {
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

const TCPStream &TCPStream::operator>>(MCP::Packet *packet) {
    packet->read(this);
    
    return *this;
};

const TCPStream &TCPStream::operator<<(MCP::Packet *packet) {
    packet->write(this);
    
    return *this;
};

const TCPStream &TCPStream::operator>>(MCP::VarInt *value) {
    int position = 0;
    int readValue = 0;
    unsigned char currentByte = 0;
    
    while ( true ) {
        recv(m_socketDescriptor, &currentByte, 1, 0); // Read current byte
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

const TCPStream &TCPStream::operator<<(MCP::VarInt *value) {
    memcpy(m_sendBuffer + m_sendBufferCurrentOffset, value->buffer(), value->size());
    m_sendBufferCurrentOffset += value->size();
    
    return *this;
};

const TCPStream &TCPStream::operator>>(std::string &value) {
    MCP::VarInt length;
    *this >> &length;
    

    recv(m_socketDescriptor, m_recvBuffer, length.value(), 0);
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
    recv(m_socketDescriptor, toReceive, 8, 0);
    
    std::reverse(value, value+sizeof(value));
    
    return *this;
};

const TCPStream &TCPStream::operator<<(int64_t value) {
    // Network order is big-endian, which happens to coincide with the Minecraft Protocol.
    value = htonl(value);
    unsigned char *toSend = reinterpret_cast<unsigned char *>(&value);
    
    memcpy(m_sendBuffer+m_sendBufferCurrentOffset, toSend, 8);
    m_sendBufferCurrentOffset += 8;
    
    return *this;
};

const TCPStream &TCPStream::operator>>(int32_t *value) {
    unsigned char *toReceive = reinterpret_cast<unsigned char *>(value);
    recv(m_socketDescriptor, toReceive, 4, 0);
    
    std::reverse(value, value+sizeof(value));
    
    return *this;
};

const TCPStream &TCPStream::operator<<(int32_t value) {
    // Network order is big-endian, which happens to coincide with the Minecraft Protocol.
    value = htonl(value);
    unsigned char *toSend = reinterpret_cast<unsigned char *>(&value);
    
    memcpy(m_sendBuffer+m_sendBufferCurrentOffset, toSend, 4);
    m_sendBufferCurrentOffset += 4;
    
    return *this;
};

const TCPStream &TCPStream::operator>>(int16_t *value) {
    unsigned char *toReceive = reinterpret_cast<unsigned char *>(value);
    recv(m_socketDescriptor, toReceive, 2, 0);
    
    std::reverse(value, value+sizeof(value));
    
    return  *this;
};

const TCPStream &TCPStream::operator<<(int16_t value) {
    value = htons(value);
    unsigned char *toSend = reinterpret_cast<unsigned char *>(&value);
    
    memcpy(m_sendBuffer+m_sendBufferCurrentOffset, toSend, 2);
    m_sendBufferCurrentOffset += 2;
    
    return *this;
};

const TCPStream &TCPStream::operator>>(bool *value) {
    recv(m_socketDescriptor, value, 1, 0);
    
    return *this;
};

const TCPStream &TCPStream::operator<<(bool value) {
    m_sendBuffer[m_sendBufferCurrentOffset] = (char) value;
    ++m_sendBufferCurrentOffset;
    
    return *this;
};


void TCPStream::test() {
    while ( 1 ) {
        memset(m_recvBuffer, 0x0, RECV_BUFFER_SIZE);
        
        ssize_t bytesRead = read(m_socketDescriptor, m_recvBuffer, RECV_BUFFER_SIZE);
        
        if ( bytesRead > 0 ) {
            printf("Received %zd bytes. Begin processing bytes:\n", bytesRead);
            
            printf("%s\n", m_recvBuffer);
            
            int currOffset = 0;
            
            MCP::VarInt length(m_recvBuffer);
            printf("Buffer length: %d\n", length.value());
            currOffset += length.size();
            
            MCP::VarInt packetId(m_recvBuffer+currOffset);
            printf("Packet Id: %d\n", packetId.value());
            currOffset += packetId.size();
            
            if ( packetId.value() == 2 ) {
                unsigned char UUID[17];
                for ( int i = 0; i < 16; i++ ) {
                    UUID[i] = m_recvBuffer[currOffset + i];
                }
                UUID[16] = '\0';
                printf("UUID: %s\n", UUID);
                currOffset += 16;
                
                MCP::VarInt nameLen(m_recvBuffer+currOffset);
                printf("Username length: %d\n", nameLen.value());
                currOffset += nameLen.size();
                
                unsigned char name[16];
                memset(name, '\0', 16);
                for ( int i = 0; i < nameLen.value(); i++ ) {
                    name[i] = m_recvBuffer[currOffset + i];
                }
                printf("Username: %s\n", name);
                currOffset += nameLen.value();
                
                MCP::VarInt propertiesNum(m_recvBuffer+currOffset);
                printf("Number of properties: %d\n", propertiesNum.value());
                
                printf("Packet processing done. Next packet.\n\n");
                
                break;
            }
        };
        
        
        
        if ( (bytesRead == -1 || bytesRead == 0 ) &&
             (errno == ENOTCONN || errno == ETIMEDOUT || errno == ECONNRESET) ) {
            break;
        }
    }
    
    AAPL_PRINT("Done.");
}
