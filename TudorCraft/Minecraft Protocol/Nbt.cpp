//
//  Nbt.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 18.12.2022.
//

#include "Nbt.hpp"

#define CheckTagToAddToCompound(T) \
    case ( (uint8_t) TagType::T ): { \
        T value;      \
        decode(stream, &value); \
        newTag.m_value = value; \
        (*val)[name] = newTag;  \
        break;                  \
    }

#define CheckTagToAddToValue(T) \
    case ( (uint8_t) TagType::T ): { \
        T value;      \
        decode(stream, &value); \
        m_value = value; \
        break;                  \
    }

using namespace MCP::NBT;

//MARK: - Encode/Decode for Tag types



//MARK: Primitives Tags
// TagByte, TagInt, TagShort, TagLong, TagFloat, TagDouble
template<class T>
void decode(Streamable *stream, T *val) {
    stream->readBytes((uint8_t *) val, sizeof(T));
    std::reverse(val, val + sizeof(T));
};

template<class T>
void encode(Streamable *stream, T *val) {
    // stub
};

//MARK: TagString
template<>
void decode(Streamable *stream, TagString *val) {
    uint16_t strLength = 0;
    stream->readBytes((uint8_t *) &strLength, 2);
#warning wtf?
    std::reverse((uint8_t *) &strLength, (uint8_t *) &strLength + 2);
    
    if ( strLength > 0 ) {
        val->resize(strLength, '\0');
        stream->readBytes((uint8_t *)val->data(), strLength);
    }
};

template<>
void encode(Streamable *stream, TagString *val) {
    // stub
};

//MARK: TagByteArray
template<>
void decode(Streamable *stream, TagByteArray *val) {
    int32_t arrayLength = 0;
    stream->readBytes((uint8_t *) &arrayLength, 4);
    std::reverse((uint8_t *) &arrayLength, (uint8_t *) &arrayLength + 4);
    
    val->resize(arrayLength);
    stream->readBytes((uint8_t *) val->data(), arrayLength);
};

template<>
void encode(Streamable *stream, TagByteArray *val) {
    // stub
};

//MARK: TagIntArray
template<>
void decode(Streamable *stream, TagIntArray *val) {
    int32_t arrayLength = 0;
    stream->readBytes((uint8_t *) &arrayLength, 4);
    std::reverse((uint8_t *) &arrayLength, (uint8_t *) &arrayLength + 4);
    
    val->resize(arrayLength);
    stream->readBytes((uint8_t *) val->data(), arrayLength * 4);
    for ( int i = 0; i < arrayLength; i++ ) {
        TagInt l = 0;
        stream->readBytes((uint8_t *) &l, 4);
        std::reverse((uint8_t *) &l, (uint8_t *) &l + 4);
        
        (*val)[i] = l;
    }
};

template<>
void encode(Streamable *stream, TagIntArray *val) {
    // stub
};

//MARK: TagLongArray
template<>
void decode(Streamable *stream, TagLongArray *val) {
    int32_t arrayLength = 0;
    stream->readBytes((uint8_t *) &arrayLength, 4);
    std::reverse((uint8_t *) &arrayLength, (uint8_t *) &arrayLength + 4);
    
    val->resize(arrayLength);
//    stream->readBytes((uint8_t *) val->data(), arrayLength * 8);
    for ( int i = 0; i < arrayLength; i++ ) {
        TagLong l = 0;
        stream->readBytes((uint8_t *) &l, 8);
        std::reverse((uint8_t *) &l, (uint8_t *) &l + 8);
        
        (*val)[i] = l;
    }
};

template<>
void encode(Streamable *stream, TagLongArray *val) {
    // stub
};

//MARK: TagCompound
#warning Replace the switch cases with a MACRO
template<>
void decode(Streamable *stream, TagCompound *val) {
    uint8_t id = 0;
    
    stream->readBytes(&id, 1);
    while ( id != (uint8_t) TagType::TagEnd ) {
        TagString name;
        decode(stream, &name);
        
        Tag newTag;
        newTag.m_name = name;
        newTag.m_type = TagType::TagEnd;
        
        switch (id) {
            case ( (uint8_t) TagType::TagByte ): {  
                TagByte value = 0;
                decode(stream, &value);
                newTag.m_value = value;
                (*val)[name] = newTag;
                break;
            }
            
            case ( (uint8_t) TagType::TagShort ): {
                TagShort value = 0;
                decode(stream, &value);
                newTag.m_value = value;
                (*val)[name] = newTag;
                break;
            }
                
            case ( (uint8_t) TagType::TagInt ): {
                TagInt value = 0;
                decode(stream, &value);
                newTag.m_value = value;
                (*val)[name] = newTag;
                break;
            }
                
            case ( (uint8_t) TagType::TagLong ): {
                TagLong value = 0;
                decode(stream, &value);
                newTag.m_value = value;
                (*val)[name] = newTag;
                break;
            }
            
            case ( (uint8_t) TagType::TagFloat ): {
                TagFloat value = 0.0f;
                decode(stream, &value);
                newTag.m_value = value;
                (*val)[name] = newTag;
                break;
            }
                
            case ( (uint8_t) TagType::TagDouble ): {
                TagDouble value = 0.0f;
                decode(stream, &value);
                newTag.m_value = value;
                (*val)[name] = newTag;
                break;
            }
                
            case ( (uint8_t) TagType::TagByteArray ): {
                TagByteArray value = {};
                decode(stream, &value);
                newTag.m_value = value;
                (*val)[name] = newTag;
                break;
            }
                
            case ( (uint8_t) TagType::TagIntArray ): {
                TagIntArray value = {};
                decode(stream, &value);
                newTag.m_value = value;
                (*val)[name] = newTag;
                break;
            }
                
            case ( (uint8_t) TagType::TagLongArray ): {
                TagLongArray value = {};
                decode(stream, &value);
                newTag.m_value = value;
                (*val)[name] = newTag;
                break;
            }
                
            case ( (uint8_t) TagType::TagString ): {
                TagString value = {};
                decode(stream, &value);
                newTag.m_value = value;
                (*val)[name] = newTag;
                break;
            }
                
            case ( (uint8_t) TagType::TagCompound ): {
                TagCompound value = {};
                decode(stream, &value);
                newTag.m_value = value;
                (*val)[name] = newTag;
                break;
            }
                
#warning "Implement TagList"
                
            default:
                break;
        }
     
        stream->readBytes(&id, 1);
    }
};

template<>
void encode(Streamable *stream, TagCompound *val) {
    // stub
};

//MARK: - Tag class implementation
void Tag::decodeFrom(Streamable *stream) {
    uint8_t id = 0;
    stream->readBytes(&id, 1);
    
    m_type = TagType::TagEnd;
    
    if ( id != (uint8_t) TagType::TagEnd ) {
        std::string name;
        decode(stream, &name);
        
        m_name = name;
        
        switch (id) {
            case ( (uint8_t) TagType::TagByte ): {
                TagByte value = 0;
                decode(stream, &value);
                m_value = value;
                break;
            }
            
            case ( (uint8_t) TagType::TagShort ): {
                TagShort value = 0;
                decode(stream, &value);
                m_value = value;
                break;
            }
                
            case ( (uint8_t) TagType::TagInt ): {
                TagInt value = 0;
                decode(stream, &value);
                m_value = value;
                break;
            }
                
            case ( (uint8_t) TagType::TagLong ): {
                TagLong value = 0;
                decode(stream, &value);
                m_value = value;
                break;
            }
            
            case ( (uint8_t) TagType::TagFloat ): {
                TagFloat value = 0.0f;
                decode(stream, &value);
                m_value = value;
                break;
            }
                
            case ( (uint8_t) TagType::TagDouble ): {
                TagDouble value = 0.0f;
                decode(stream, &value);
                m_value = value;
                break;
            }
                
            case ( (uint8_t) TagType::TagByteArray ): {
                TagByteArray value = {};
                decode(stream, &value);
                m_value = value;
                break;
            }
                
            case ( (uint8_t) TagType::TagIntArray ): {
                TagIntArray value = {};
                decode(stream, &value);
                m_value = value;
                break;
            }
                
            case ( (uint8_t) TagType::TagLongArray ): {
                TagLongArray value = {};
                decode(stream, &value);
                m_value = value;
                break;
            }
                
            case ( (uint8_t) TagType::TagString ): {
                TagString value = {};
                decode(stream, &value);
                m_value = value;
                break;
            }
                
            case ( (uint8_t) TagType::TagCompound ): {
                TagCompound value = {};
                decode(stream, &value);
                m_value = value;
                break;
            }
                
#warning "Implement TagList"
        }
    }
};

void Tag::encodeTo(Streamable *stream) {
    // stub
};
