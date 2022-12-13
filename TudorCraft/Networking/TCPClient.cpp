//
//  TCPClient.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 01.12.2022.
//

#include "TCPClient.hpp"

TCPClient::TCPClient(std::string serverAddress, int port) : m_server(&m_state, serverAddress, port) {
    std::string name = "TudorCraft";
    
    MCP::HandshakePacket initialHandshake(760, serverAddress, port);
    MCP::LoginStartPacket loginStart(name);
    
    m_server << &initialHandshake;
    m_server << &loginStart;
    
    m_state = MCP::ConnectionState::Handshaking;
    
    m_recvThread = std::thread(&TCPClient::readLoop, this);
};


TCPClient::~TCPClient() {
    
    m_recvThread.join();
    AAPL_PRINT("TCPClient done!");
};

void TCPClient::readLoop() {
    while ( true ) {
        if ( m_state == MCP::ConnectionState::Unconnected ) {
            break;
        }
        
        MCP::Packet *receivedPacket = nullptr;
        
        m_server >> receivedPacket;
        
        if ( receivedPacket != nullptr ) {
            delete receivedPacket;
        }
    }
    
    AAPL_PRINT("Socket Done.");
};
