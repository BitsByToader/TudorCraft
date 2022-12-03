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
    TCPStream m_server;
    
    
    std::string m_serverAddress;
    int m_serverPort;
    
    
    
    unsigned char *m_writeBuffer;
    unsigned char *m_recvBuffer;
};

#endif /* TCPClient_hpp */
