#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <cstdint>

namespace PbrGi {
    class Texture {
        public:
            Texture(unsigned int textureId = 0);
            ~Texture();
            
            bool getTextureId(unsigned int& textureId) {
                textureId = mTextureId;
                return mTextureIfValid;
            }

            bool init2DTexture(unsigned int width, unsigned int height, unsigned int format, bool mipmap);
            bool init2DTexture(const unsigned char* imageData, unsigned int size, bool mipmap);
            bool init2DTexture(std::string path, bool mipmap);
            bool init2DTexture(std::vector<std::string> paths, unsigned int mipmapLevel);
            bool initCubeTexture(std::vector<std::string> paths, bool mimmap);
            bool initCubeTextureHDR(std::vector<std::string> paths, unsigned int mipmapLevel);
            bool init2DTextureHDR(std::string path, bool mipmap);

            bool TestInitCubeTextureHDR(std::vector<std::string> paths, unsigned int mipmapLevel);

        private:
            unsigned int mTextureId;
            bool mTextureIfValid;
    };
}

#endif