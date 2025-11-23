#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

namespace PbrGi {

Texture::Texture() {

}

Texture::~Texture() {

}

bool Texture::init2DTexture(const unsigned char* imageData, unsigned int size, bool mipmap) {
    glGetError();

    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_2D, mTextureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (mipmap) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    int width, height, nrChannels;
    unsigned char* data = stbi_load_from_memory(imageData, size, &width, &height, &nrChannels, 0);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        if (mipmap) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }
    else {
        stbi_image_free(data);
        mTextureIfValid = false;
        glBindTexture(GL_TEXTURE_2D, 0);
        return false;
    }

    if (0 == glGetError()) {
        stbi_image_free(data);
        mTextureIfValid = true;
        glBindTexture(GL_TEXTURE_2D, 0);
        return true;
    }
    else {
        stbi_image_free(data);
        mTextureIfValid = false;
        glBindTexture(GL_TEXTURE_2D, 0);
        return false;
    }    
}

bool Texture::init2DTexture(std::string path, bool mipmap) {
    glGetError();

    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_2D, mTextureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (mipmap) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        if (mipmap) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }
    else {
        stbi_image_free(data);
        mTextureIfValid = false;
        glBindTexture(GL_TEXTURE_2D, 0);
        return false;
    }

    if (0 == glGetError()) {
        stbi_image_free(data);
        mTextureIfValid = true;
        glBindTexture(GL_TEXTURE_2D, 0);
        return true;
    }
    else {
        stbi_image_free(data);
        mTextureIfValid = false;
        glBindTexture(GL_TEXTURE_2D, 0);
        return false;
    }
}

bool Texture::init2DTexture(std::vector<std::string> paths, unsigned int mipmapLevel) {
    return false;
}

bool Texture::initCubeTexture(std::vector<std::string> faces, bool mimmap) {
    glGetError();

    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureId);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    if (mimmap) {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    if (0 == glGetError()) {
        mTextureIfValid = true;
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        return true;
    }
    else {
        mTextureIfValid = false;
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        return false;
    }
}

bool Texture::initCubeTextureHDR(std::vector<std::string> faces, unsigned int mipmapLevel) {
    glGetError();

    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureId);

    int baseWidth, baseHeight, nrChannels;

    for (unsigned int i = 0; i < 6; i++)
    {
        float* data = stbi_loadf(faces[i].c_str(), &baseWidth, &baseHeight, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB16F, baseWidth, baseHeight, 0, GL_RGB, GL_FLOAT, data
            );
            stbi_image_free(data);
            data = nullptr;
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, mipmapLevel - 1);

    //glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    for (int level = 0; level < mipmapLevel; level++) {
        for (unsigned int i = 0; i < 6; i++)
        {
            int width, height, channels;
            float* data = stbi_loadf(faces[level * 6 + i].c_str(), &width, &height, &channels, 0);
            if (((baseWidth >> level) != width) || ((baseHeight >> level) != height)) {
                std::cout << "baseWidth or baseHeight is not match mipmap level" << std::endl;
            }
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
                stbi_image_free(data);
                data = nullptr;
            }
            else
            {
                std::cout << "Cubemap texture failed to load at path: " << faces[level * 6 + i] << std::endl;
                stbi_image_free(data);
                data = nullptr;
            }
        }
    }



    if (0 == glGetError()) {
        mTextureIfValid = true;
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        return true;
    }
    else {
        mTextureIfValid = false;
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        return false;
    }
}

bool Texture::TestInitCubeTextureHDR(std::vector<std::string> faces, unsigned int mipmapLevel) {
    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureId);   

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, mipmapLevel - 1);

    int baseWidth, baseHeight, nrChannels;
    stbi_loadf(faces[0].c_str(), &baseWidth, &baseHeight, &nrChannels, 0);

    glTexStorage2D(GL_TEXTURE_CUBE_MAP, mipmapLevel, GL_RGB16F, baseWidth, baseHeight);

    for (int level = 0; level < mipmapLevel; level++) {
        for (unsigned int i = 0; i < 6; i++)
        {
            int width, height, channels;
            float* data = stbi_loadf(faces[level * 6 + i].c_str(), &width, &height, &channels, 0);
            if (((baseWidth >> level) != width) || ((baseHeight >> level) != height)) {
                std::cout << "baseWidth or baseHeight is not match mipmap level" << std::endl;
            }
            if (data)
            {
                glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level, 0, 0, width, height, GL_RGB, GL_FLOAT, data);
                stbi_image_free(data);
                data = nullptr;
            }
            else
            {
                std::cout << "Cubemap texture failed to load at path: " << faces[level * 6 + i] << std::endl;
                stbi_image_free(data);
                data = nullptr;
            }
        }
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return true;
}


bool Texture::init2DTextureHDR(std::string path, bool mipmap) {
    glGetError();

    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_2D, mTextureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (mipmap) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    int width, height, nrChannels;
    float* data = stbi_loadf(path.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
        if (mipmap) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }
    else {
        stbi_image_free(data);
        mTextureIfValid = false;
        glBindTexture(GL_TEXTURE_2D, 0);
        return false;
    }

    if (0 == glGetError()) {
        stbi_image_free(data);
        mTextureIfValid = true;
        glBindTexture(GL_TEXTURE_2D, 0);
        return true;
    }
    else {
        stbi_image_free(data);
        mTextureIfValid = false;
        glBindTexture(GL_TEXTURE_2D, 0);
        return false;
    }
}
}