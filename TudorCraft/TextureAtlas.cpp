//
//  TextureAtlas.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 05.11.2022.
//

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#import <TargetConditionals.h>
#import <Availability.h>
#include <Foundation/Foundation.hpp>
#include "AAPLUtilities.h"

#include "TextureAtlas.hpp"

TextureAtlas::~TextureAtlas() {
    stbi_image_free(m_rawData);
}

void TextureAtlas::loadAtlasInMemory() {
    NS::Bundle *mainBundle = NS::Bundle::mainBundle();
    
    NS::String *imgPath = NS::String::alloc()->init(mainBundle->resourcePath());
    NS::String *imgName = NS::String::alloc()->init("/test.png", NS::StringEncoding::UTF8StringEncoding);
    imgPath = imgPath->stringByAppendingString(imgName);
    
    // 4 -> 4 channels -> Red, Green, Blue, Alpha
    m_rawData = stbi_load(imgPath->utf8String(), &m_width, &m_height, &m_channels, 4);
    
    if ( m_rawData == nullptr ) {
        AAPL_ASSERT(true, "Couldn't load texture atlas!");
    }
    
#ifdef TARGET_OS_MAC
    AAPL_PRINT("Swapping r and b channels for macOS...");
    
    for ( int i = 0; i < m_height; i++ ) {
        for ( int j = 0; j < m_width * m_channels; j += m_channels ) {
            int iteration = i * m_height + j;
            
            int cpy = m_rawData[iteration]; //save r
            m_rawData[iteration] = m_rawData[iteration + 2]; // r = b
            m_rawData[iteration + 2] = cpy; // b = r
        }
    }
#endif
};

unsigned char *TextureAtlas::getRawData() {
    return m_rawData;
};

int TextureAtlas::getWidth() {
    return m_width;
};

int TextureAtlas::getHeight() {
    return m_height;
};

int TextureAtlas::getChannels() {
    return m_channels;
};
