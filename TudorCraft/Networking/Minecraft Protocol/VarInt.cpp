//
//  MCPVarInt.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 01.12.2022.
//

#include "VarInt.hpp"

using namespace MCP;

VarInt::VarInt() {
    memset(m_bytes, 0x0, MAX_SIZE);
    
    m_size = 1;
    m_value = 0;
};

VarInt::VarInt(int value) {
    memset(m_bytes, 0x0, MAX_SIZE);
    
    m_value = value;
    int position = 0;
    
    while ( true ) {
        if ( (value & ~SEGMENT_BITS) == 0 ) {
            // If the MSB is 0, then we don't have anything left to put in the buffer
            m_bytes[position] = value;
            
            break;
        }
        
        m_bytes[position] = (value & SEGMENT_BITS) | CONTINUE_BIT;
        
        value >>= 7;
        position++;
    }
    
    m_size = position+1;
};

VarInt::VarInt(const unsigned char *buffer) {
    memset(m_bytes, 0x0, MAX_SIZE);
    
    int position = 0;
    
    while ( true ) {
        m_bytes[position] = buffer[position];
        m_value |= ( buffer[position] & SEGMENT_BITS ) << position*7;
        
        if ( (buffer[position] & CONTINUE_BIT) == 0 ) {
            break;
        }
        
        ++position;
        
        if ( position*7 >= 32 ) {
            AAPL_PRINT("VarInt is too big.");
            break;
        }
    }
    
    m_size = position+1;
};

int VarInt::size() {
    return m_size;
};

const unsigned char *VarInt::buffer() {
    return m_bytes;
};

int VarInt::value() {
    return m_value;
};
