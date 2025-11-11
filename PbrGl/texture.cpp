#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

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

bool Texture::init3DTexture(std::vector<std::string> faces, bool mimmap) {
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

bool Texture::init3DTexture(std::vector<std::string> faces, unsigned int mipmapLevel) {
    glGetError();

    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureId);

    int baseWidth, baseHeight, nrChannels;
    for (unsigned int i = 0; i < 6; i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &baseWidth, &baseHeight, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, baseWidth, baseWidth, 0, GL_RGB, GL_UNSIGNED_BYTE, data
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

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmapLevel);

    for (int level = 0; level < mipmapLevel; level++) {
        for (unsigned int i = 0; i < 6; i++)
        {
            int width, height, channels;
            unsigned char* data = stbi_load(faces[level * 6 + i].c_str(), &width, &height, &channels, 0);
            if (((baseWidth >> level) != width) || ((baseHeight >> level) != height)) {
                std::cout << "baseWidth or baseHeight is not match mipmap level" << std::endl;
            }
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    level, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                );
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
                stbi_image_free(data);
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

}