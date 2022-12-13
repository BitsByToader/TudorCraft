//
//  TCPClient.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 01.12.2022.
//

#ifndef TCPClient_hpp
#define TCPClient_hpp

// Std Library
#include <string>
#include <thread>
#include <typeinfo>

#include "AAPLUtilities.h"
#include "VarInt.hpp"
#include "TCPStream.hpp"
#include "Packet.hpp"

class TCPClient {
public:
    //MARK: - Public methods
    TCPClient(std::string serverAddress, int serverPort);
    ~TCPClient();
    
private:
    //MARK: - Private methods
    void readLoop();
    
    //MARK: - Private members
    MCP::ConnectionState m_state = MCP::ConnectionState::Unconnected;
    TCPStream m_server;
    
    std::thread m_recvThread;
};

#endif /* TCPClient_hpp */
