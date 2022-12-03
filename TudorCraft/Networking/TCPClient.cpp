//
//  TCPClient.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 01.12.2022.
//

#include "TCPClient.hpp"

TCPClient::TCPClient(std::string serverAddress, int port) : m_server(serverAddress, port) {
    std::string name = "TudorCraft";
    
    MCP::HandshakePacket initialHandshake(760, serverAddress, port);
    MCP::LoginStartPacket loginStart(name);
    
    m_server << &initialHandshake;
    m_server << &loginStart;
    
    MCP::LoginSuccessPacket loginSuccess;
    
    m_server >> &loginSuccess;
};


TCPClient::~TCPClient() {
    AAPL_PRINT("TCPClient done!");
};
