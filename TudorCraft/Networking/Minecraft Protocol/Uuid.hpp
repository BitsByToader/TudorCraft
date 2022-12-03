//
//  Uuid.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 03.12.2022.
//

#ifndef Uuid_hpp
#define Uuid_hpp

#include <cstdint>

namespace MCP {
class Uuid {
public:
    Uuid() {
        mostSignificant = 0;
        leastSignificat = 0;
    }
    
    int64_t mostSignificant;
    int64_t leastSignificat;
};
}

#endif /* Uuid_hpp */
