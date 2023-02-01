# Introduction
*TudorCraft* is a client for any vanilla Minecraft Java Edition 1.19.2 server. The goal was to create a fast, easy-on-resources clone of Minecraft that can run iPhones, iPads and Macs. Thus, it was written in C++ using Apple's graphics API, Metal, or more specifically, metal-cpp (you can read more about metal-cpp [here](metal-cpp/README.md)).

# Table of contents:
1. [Features](#features)
2. [System requirements](#system-requirements)
3. [Technical Decisions](#technical-decisions)
4. [The Minecraft Protocol](#the-minecraft-protocol)
	- 4.1. [Network protocol](#network-protocol)
	- 4.2. [Named Binary Tag](#named-binary-tag)
5. [Engine and renderer design](#engine-and-renderer-design)
6. [Where to go next](#where-to-go-next)

# Features
- A velocity-based physics engine with full collision support.
- Voxel-based 3D renderer written from scratch making use of instancing, back-face culling, depth-ordering, etc in the Apple Metal API.
- Multiplatform support for iOS, iPadOS and macOS, with both touch controls and keyboard and mouse support.
- Beautiful UI written in SwiftUI that communicates via IPC (Apple’s NotificationCenter) with the   C++ game engine to send UI updates.
- Ability to connect to a vanilla Minecraft: Java Edition 1.19.2 server.
- Full entity and world support with the server.
- Fast and low memory usage compared to other clients (although the game doesn't implement most of the features of the original game).

# System requirements
TudorCraft can run on any device running at least iOS 16.0 or macOS 13.0. The game can connect to any vanilla(=unmodified) Minecraft 1.19.2 server provided it is set to offline mode and compression is disabled. 

# Technical Decisions
The project's goal, from the start, was to make a Minecraft client for any vanilla server in C++. This meant that the code was supposed to run on multiple devices, so a multi-platform codebase was the only solution, given the timeframe. While Qt was the first idea, the lack of support of modern OpenGL on Apple's platforms meant that it was a no-go. After discovering metal-cpp, it was the obivous choice for writing the 3D renderer, given its fine control over the GPU hardware.

While it was possible to port the rest of Apple's UI frameworks to C++ to make the project pure C++ just like how Metal was ported in a header-only fashion from Objective-C, the time constraints didn't make it feasible. As such, the rest of the UI for the game was made in Apple's declarative UI framework, [SwiftUI](https://developer.apple.com/xcode/swiftui/), and [NotificationCenter](https://developer.apple.com/documentation/foundation/notificationcenter) was used for the C++ codebase and SwiftUI to transfer messages via IPC.

# The Minecraft Protocol
## Network protocol
Connecting to a vanilla Minecraft server means conforming to the protocol that the original client and server developed by Mojang use. I had a lot of help from [here](https://wiki.vg/Protocol), as it provided detailed information on how the protocol works and the data types used.

As a quick summary, the server is nothing more than a TCP server, especially when it is setup in offline mode and with compression disabled. The protocol defines a standard packet format for all packet types, where the length is sent first, then the packet id, and finally, the actual packet data. Thus a client can choose to support whatever packet types it chooses and can easily skip what it doesn't. 

Another thing to note is the special data types the protocol uses. While there the primitive types that are all sent in big-endian order, the protocol also uses `VarInt`s and `VarLong`s, special data types that use MSB of every byte to indicate whether or not there is a following byte. This allows the types to be basically be, as the name implies, a variable length version of `int` and `long`. There are also other special types for specific purposes, but they aren't as common an can be viewed [here](https://wiki.vg/Protocol).

The network protocol's implementation can be seen [here](TudorCraft/Networking/TCPStream.hpp) for the actual TCP client and [here](TudorCraft/Minecraft%20Protocol/Packet.hpp) for the implemented packet types.

## Named Binary Tag
Minecraft also uses another proprietary format, called *Named Binary Tag* for sending more complex data over the network without over-complicating the network protocol or for storing arbitrary data in an organized fashion in persistent storage. As the name implies, the format is similar to JSON, the main difference being that the data isn't represented as an UTF-8 string, but in binary. Thus, every tag's data is preceded by its tag id, the length of the name and the actual name.

Generally, every NBT file or data will implicitly be inside of a `TagCompound` and end with its pair of `TagEnd`. A compound can hold any other type of tags such as `TagByte`, `TagInt`, `TagIntArray`, `TagList` (which is just a nameless compound with a predefined number of tags), etc. It's a really simple format and I wrote a decoder from scratch. While it only reads NBTs from the network, it can also easily read from files as well, provided there is a file reading class which inherits the `Streamable` protocol.

More details are provided [here](https://wiki.vg/NBT). Implementation can be seen [here](TudorCraft/Minecraft%20Protocol/Nbt.hpp).

# Engine and renderer design
The engine design is directly correlated to the memory model used. The engine being written in C++20 means that smart pointers are available and encouraged. This wasn't taken lightly when writing the game, as smart pointers are used in most places in the codebase, however here are two types used.

The renderer uses Apple's flavor of smart pointers (which feature only manual reference counting (MRC) in C++). This is because all of the objects used in the renderer inherit `NSObject`, which require the use of reference counting for them to properly be allocated and deallocated. Because of this, the code makes use of `retain`, `release` and `AutoreleasePool`s to properly manage the lifetime of GPU related objects. More information on memory management in C++ code when using Apple frameworks can be found [here](metal-cpp/README.md).

The engine uses the C++ standard library's smart pointers to manage it's resources. The engine is in a very early state, it doesn't pack a lot of features, so there aren't many resources to manage, but the current design makes it very easy to add more complexity on top of it. The use of smart pointers prevented me from needing to write a complex entity component system to properly manage the lifetime of all the components of the game. As such, there can be many copies of `shared_ptr`s of derived classes for different purposes like collision checking, applying physics or rendering, but the underlying object will only be deleted when it's not needed thanks to the automatic reference counting. This also made it easy to write asynchronous code without use-after-free bugs, the game running on multiple threads for functionality like networking, receiving input and inter-process communication with the UI codebase. 

# Where to go next
This README only provides an introduction to the codebase and all the choices that went into making this game, so there are many details that where left out. I urge you to have a look through the code, or check out the [Doxygen generated documentation](https://bitsbytoader.github.io/TudorCraft/).
