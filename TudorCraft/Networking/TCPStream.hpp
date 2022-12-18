//
//  TCPStream.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 02.12.2022.
//

#ifndef TCPStream_hpp
#define TCPStream_hpp

// Std Library
#include <cstdint>
#include <string>
#include <thread>

// Networking
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "AAPLUtilities.h"
#include "PacketTypes.hpp"
#include "Packet.hpp"
#include "VarInt.hpp"
#include "Uuid.hpp"

#define SEND_BUFFER_SIZE 1024
#define RECV_BUFFER_SIZE 2097152

class TCPStream {
public:
    TCPStream() {};
    TCPStream(MCP::ConnectionState *state, std::string serverAddress, int port);
    ~TCPStream();
    
    void flushOutput();
    void flushInput();
    
    long receiveFromSocket(void *buffer, int64_t length);
    
#warning "Question: Pointer vs Reference?"
    
    template<class T>
    const TCPStream &operator>>(T *value);
    template<class T>
    const TCPStream &operator<<(T *value);
    
    template<>
    const TCPStream &operator>>(MCP::Packet *packet);
    template<>
    const TCPStream &operator<<(MCP::Packet *packet);
    
    template<>
    const TCPStream &operator>>(MCP::VarInt *value);
    template<>
    const TCPStream &operator<<(MCP::VarInt *value);
    
    template<>
    const TCPStream &operator>>(MCP::Property *property);
    template<>
    const TCPStream &operator<<(MCP::Property *property);
    
    template<>
    const TCPStream &operator>>(std::string *value);
    template<>
    const TCPStream &operator<<(std::string *value);
    
    template<>
    const TCPStream &operator>>(MCP::Uuid *value);
    template<>
    const TCPStream &operator<<(MCP::Uuid *value);
    
private:
    MCP::ConnectionState *m_connectionState;
    int m_socketDescriptor;
    
    unsigned char m_sendBuffer[SEND_BUFFER_SIZE];
    int m_sendBufferCurrentOffset = 0;
    
    unsigned char m_recvBuffer[RECV_BUFFER_SIZE];
    int m_recvBufferCurrentOffset = 0;
};

#endif /* TCPStream_hpp */
