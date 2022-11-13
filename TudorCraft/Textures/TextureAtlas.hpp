//
//  TextureAtlas.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 05.11.2022.
//

#ifndef TextureAtlas_hpp
#define TextureAtlas_hpp

//MARK: TODO
/*
 - Make a static function to get the base resource path
 - Refactor the loadAtlasInMemory method to have the image name as a paramater
 - Implement the rest of the functionality of the atlas
 */

/// This class will load one singular PNG in memory.
/// Using predefined indexes for each texture, other objects can then get a pointer of individual textures.
class TextureAtlas {
public:
    ~TextureAtlas();
    
    /// Loads the atlas in rawData using the file path provided in the constructor
    void loadAtlasInMemory();
    
    /// Getter for the raw data (intermediate function until I make a proper atlas)
    unsigned char *getRawData();
    
    /// Getter for the width (intermediate function until I make a proper atlas)
    int getWidth();
    
    /// Getter for the height (intermediate function until I make a proper atlas)
    int getHeight();
    
    /// Getter for the channels (intermediate function until I make a proper atlas)
    int getChannels();
    
private:
    const char *m_filePath = nullptr;
    unsigned char *m_rawData = nullptr;
    int m_width = 0;
    int m_height = 0;
    int m_channels = 0;
};

#endif /* TextureAtlas_hpp */
