//
//  MCPVarInt.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 01.12.2022.
//

#ifndef MCPVarInt_hpp
#define MCPVarInt_hpp

#include "AAPLUtilities.h"

#define MAX_SIZE 5
#define SEGMENT_BITS 0x7F
#define CONTINUE_BIT 0x80

namespace MCP {

class VarInt {
public:
    //MARK: - Constructors
    VarInt();
    VarInt(int value);
    VarInt(const unsigned char *buffer);
    VarInt(const unsigned char *buffer, int value, int size);
    
    //MARK: - Public methods
    /// Returns the size of the VarInt in bytes.
    int size();
    
    /// Returns a read only buffer
    const unsigned char *buffer();
    
    /// Returns the value of the VarInt.
    int value();
    
private:
    //MARK: - Pricvate members
    int m_size = 0;
    int m_value = 0;
    unsigned char m_bytes[MAX_SIZE];
};

}


#endif /* MCPVarInt_hpp */
