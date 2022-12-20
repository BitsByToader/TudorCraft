//
//  Nbt.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 18.12.2022.
//

#ifndef Nbt_hpp
#define Nbt_hpp

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <unordered_map>
#include <variant>

#include "Streamable.hpp"

namespace MCP {

namespace NBT {

//Forward declaration
class Tag;

//MARK: - TagType enum
enum class TagType: uint8_t {
    TagEnd          = 0,
    TagByte         = 1,
    TagShort        = 2,
    TagInt          = 3,
    TagLong         = 4,
    TagFloat        = 5,
    TagDouble       = 6,
    TagByteArray    = 7,
    TagString       = 8,
    TagList         = 9,
    TagCompound     = 10,
    TagIntArray     = 11,
    TagLongArray    = 12
};

//MARK: - Tag typedefs
typedef std::nullptr_t                      TagEnd;
typedef int8_t                              TagByte;
typedef int16_t                             TagShort;
typedef int32_t                             TagInt;
typedef int64_t 	                        TagLong;
typedef float                               TagFloat;
typedef double                              TagDouble;
typedef std::vector<TagByte>                TagByteArray;
typedef std::vector<TagInt>                 TagIntArray;
typedef std::vector<TagLong>                TagLongArray;
typedef std::string                         TagString;
typedef std::unordered_map<TagString, Tag>  TagCompound;
typedef std::vector<Tag>                    TagList;

//MARK: - Main Tag class
class Tag {
public:
    void decodeFrom(Streamable *stream);
    void encodeTo(Streamable *stream);
    
public: // TODO: Change me!
    TagType m_type;
    TagString m_name;
    std::variant<TagEnd, TagByte, TagShort, TagInt, TagLong, TagFloat, TagDouble, TagByteArray, TagIntArray, TagLongArray, TagString, TagCompound, TagList> m_value;
};

//MARK: - Static decode methods
//template<class T>
//void decode(Streamable *stream, T *val);
//
//template<class T>
//void encode(Streamable *stream, T *val);

};

};

#endif /* Nbt_hpp */
