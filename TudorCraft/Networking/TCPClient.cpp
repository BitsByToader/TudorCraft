//
//  TCPClient.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 01.12.2022.
//

#include "TCPClient.hpp"

#include "Engine.hpp"

TCPClient::TCPClient(std::string username, std::string serverAddress, int serverPort) : m_server(&m_state, serverAddress, serverPort) {
    
    MCP::HandshakePacket initialHandshake(760, serverAddress, serverPort);
    MCP::LoginStartPacket loginStart(username);
    
    m_server << reinterpret_cast<MCP::Packet *>(&initialHandshake);
    m_server << reinterpret_cast<MCP::Packet *>(&loginStart);
    
    m_state = MCP::ConnectionState::Handshaking;
    
    m_recvThread = std::thread(&TCPClient::readLoop, this);
    
    Engine::shared()->setServerConnection(this);
};


TCPClient::~TCPClient() {
    m_state = MCP::ConnectionState::Unconnected;
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

void TCPClient::sendPacket(MCP::Packet *p) {
    if ( m_state == MCP::ConnectionState::Playing )
        m_server << p;
};
