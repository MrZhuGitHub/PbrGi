#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <string>
#include <vector>

namespace PbrGi {
    class Texture {
        public:
            Texture();
            ~Texture();
            
            bool getTextureId(unsigned int& textureId) {
                textureId = mTextureId;
                return mTextureIfValid;
            }

            bool init2DTexture(const unsigned char* imageData, unsigned int size, bool mipmap);
            bool init2DTexture(std::string path, bool mimmap);
            bool init2DTexture(std::vector<std::string> paths, unsigned int mipmapLevel);
            bool initCubeTexture(std::vector<std::string> paths, bool mimmap);
            bool initCubeTexture(std::vector<std::string> paths, unsigned int mipmapLevel);

        private:
            unsigned int mTextureId;
            bool mTextureIfValid;
    };
}

#endif