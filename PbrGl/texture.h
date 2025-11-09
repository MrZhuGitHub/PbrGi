#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <string>
#include <vector>

namespace PbrGi {
    class Texture {
        public:
            Texture();
            ~Texture();
            
            bool getTextureId(int& textureId) const {
                textureId = mTextureId;
                return mTextureIfValid;
            }

            bool init2DTexture(std::string path, bool mimmap);
            bool init2DTexture(std::vector<std::string> paths, unsigned int mipmapLevel);
            bool init3DTexture(std::vector<std::string> paths, bool mimmap);
            bool init3DTexture(std::vector<std::string> paths, unsigned int mipmapLevel);

        private:
            unsigned int mTextureId;
            bool mTextureIfValid;
    };
}

#endif