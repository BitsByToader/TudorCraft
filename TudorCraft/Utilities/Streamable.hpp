//
//  Streamable.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 19.12.2022.
//

#ifndef Streamable_h
#define Streamable_h

#include <cstdint>

/// Useful interface for abstracting the reading and writing of data from whatever stream-based source there is.
/// Override the `readBytes` and `writeBytes` to enable communication between whatever class and your datasource.
///
/// Examples: TCP/UDP sockets, file sockets, I/O streams etc.
class Streamable {
public:
    virtual size_t readBytes(uint8_t *buffer, size_t length) = 0;
    virtual size_t writeBytes(uint8_t * buffer, size_t length) = 0;
};

#endif /* Streamable_h */
