//
//  TextureAtlas.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 05.11.2022.
//

#ifndef TextureAtlas_hpp
#define TextureAtlas_hpp

#include <Metal/Metal.hpp>

#define TEXTURE_WIDTH 16
#define TEXTURE_HEIGHT 16
#define ATLAS_FILENAME "/textures.png"

/// This class will load one singular PNG in memory.
/// Using predefined indexes for each texture, other objects can then get a `MTL::Texture` object for every texture.
class TextureAtlas {
public:
    TextureAtlas(MTL::Device *device);
    ~TextureAtlas();
    
    /// Loads the atlas in rawData using the file path provided in the constructor.
    void loadAtlasInMemory();
    
    /// Makes a MTLTexture from image data from the atlas at the specified coordinates.
    MTL::Texture *getTextureWithCoordinates(int x, int y);
    
private:
    MTL::Device *m_device = nullptr;
    
    unsigned char *m_atlasRawData = nullptr;
    unsigned char *m_textureRawData = nullptr;
    
    int m_atlasWidth = 0;
    int m_atlasHeight = 0;
    int m_channels = 0;
};

#endif /* TextureAtlas_hpp */
