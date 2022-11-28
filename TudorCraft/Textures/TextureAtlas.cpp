//
//  TextureAtlas.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 05.11.2022.
//

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <Metal/Metal.hpp>

#include <TargetConditionals.h>
#include <Availability.h>
#include <Foundation/Foundation.hpp>
#include "AAPLUtilities.h"

#include "TextureAtlas.hpp"

//MARK: - Constructor
TextureAtlas::TextureAtlas(MTL::Device *device): m_device(device) {
}

//MARK: - Destructor
TextureAtlas::~TextureAtlas() {
    stbi_image_free(m_atlasRawData);
    delete []m_textureRawData;
}

//MARK: - Atlas Loader
void TextureAtlas::loadAtlasInMemory() {
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();
    
    NS::Bundle *mainBundle = NS::Bundle::mainBundle();
    
    NS::String *imgPath = NS::String::alloc()->init(mainBundle->resourcePath());
    NS::String *imgName = NS::String::alloc()->init(ATLAS_FILENAME, NS::StringEncoding::UTF8StringEncoding);
    imgPath = imgPath->stringByAppendingString(imgName);
    
    AAPL_PRINT("Texture atlas path:", imgPath->utf8String());
    
    // 4 -> 4 channels -> Red, Green, Blue, Alpha
    m_atlasRawData = stbi_load(imgPath->utf8String(), &m_atlasWidth, &m_atlasHeight, &m_channels, 4);
    AAPL_ASSERT(m_atlasRawData, "Couldn't load texture atlas!");
    
    m_textureRawData = new unsigned char[TEXTURE_WIDTH * TEXTURE_HEIGHT * m_channels];
    
    //TODO: Investigate the commented weirdness
//    imgPath->release(); This one causes an use-after-free error for some reason
//    imgName->release();
    pPool->release();
};

//MARK: - Texture Loader
MTL::Texture *TextureAtlas::getTextureWithCoordinates(int x, int y) {
    for ( int i = 0; i < 16; i++ ) {
        for ( int j = 0; j < 16; j++ ) {
            // Swap red with blue channels
#if TARGET_OS_IPHONE
            m_textureRawData[(i*16+j)*4 + 0] = m_atlasRawData[((x+i) * m_atlasHeight + (y+j)) * 4 + 0];
            m_textureRawData[(i*16+j)*4 + 1] = m_atlasRawData[((x+i) * m_atlasHeight + (y+j)) * 4 + 1];
            m_textureRawData[(i*16+j)*4 + 2] = m_atlasRawData[((x+i) * m_atlasHeight + (y+j)) * 4 + 2];
            m_textureRawData[(i*16+j)*4 + 3] = m_atlasRawData[((x+i) * m_atlasHeight + (y+j)) * 4 + 3];
#else
            m_textureRawData[(i*16+j)*4 + 0] = m_atlasRawData[((x+i) * m_atlasHeight + (y+j)) * 4 + 2];
            m_textureRawData[(i*16+j)*4 + 1] = m_atlasRawData[((x+i) * m_atlasHeight + (y+j)) * 4 + 1];
            m_textureRawData[(i*16+j)*4 + 2] = m_atlasRawData[((x+i) * m_atlasHeight + (y+j)) * 4 + 0];
            m_textureRawData[(i*16+j)*4 + 3] = m_atlasRawData[((x+i) * m_atlasHeight + (y+j)) * 4 + 3];
#endif
        }
    }
    
    // Indicate that each pixel has a blue, green, red, and alpha channel, where each channel is
    MTL::TextureDescriptor *textureDescriptor = MTL::TextureDescriptor::alloc()->init();
    textureDescriptor->setPixelFormat(MTL::PixelFormatBGRA8Unorm_sRGB);
    
    // Set the pixel dimensions of the texture
    textureDescriptor->setWidth(TEXTURE_WIDTH);
    textureDescriptor->setHeight(TEXTURE_HEIGHT);
    
    NS::UInteger bytesPerRow = m_channels * TEXTURE_WIDTH;
    
    // Create the texture from the device by using the descriptor
    MTL::Texture *texture = m_device->newTexture(textureDescriptor);
    
    MTL::Region region = MTL::Region(0, 0, 0, // MTLOrigin
                                     16, 16, 1); // MTLSize
    
    // Copy the bytes from the data object into the texture
    texture->replaceRegion(region, 0, m_textureRawData, bytesPerRow);
    
    textureDescriptor->release();
    
    return texture;
};
