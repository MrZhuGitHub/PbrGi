#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

#include <math.h>

namespace PbrGi {

Texture::Texture() {

}

Texture::~Texture() {

}

bool Texture::init2DTexture(const unsigned char* imageData, unsigned int size, bool mipmap) {
    // 清除之前的错误
    while (glGetError() != GL_NO_ERROR);

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
    if (!data) {
        // 图像加载失败，清理并返回
        mTextureIfValid = false;
        glBindTexture(GL_TEXTURE_2D, 0);
        return false;
    }

    // 关键修正1：计算Mipmap层级，但即使不用Mipmap，层级数至少为1
    int mipmapLevels = 1; // 默认不使用Mipmap时，至少1级
    if (mipmap) {
        mipmapLevels = 1 + floor(log2(std::max(width, height)));
    }

    GLenum internalFormat = GL_RGB; // 默认格式
    GLenum dataFormat = GL_RGB;

    // 根据通道数选择格式
    if (nrChannels == 1) {
        // 关键修正2：使用更标准的单通道内部格式
        internalFormat = GL_R8;    // 或者使用 GL_RED
        dataFormat = GL_RED;
    }
    else if (nrChannels == 3) {
        internalFormat = GL_RGB8;
        dataFormat = GL_RGB;
    }
    else if (nrChannels == 4) {
        internalFormat = GL_RGBA8;
        dataFormat = GL_RGBA;
    }

    // 关键修正3：使用计算出的层级数（至少为1）分配纹理存储
    glTexStorage2D(GL_TEXTURE_2D, mipmapLevels, internalFormat, width, height);

    // 检查纹理存储是否成功
    if (glGetError() != GL_NO_ERROR) {
        stbi_image_free(data);
        mTextureIfValid = false;
        glBindTexture(GL_TEXTURE_2D, 0);
        return false;
    }

    // 上传图像数据到第0级Mipmap
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);

    // 关键修正4：如果启用Mipmap，则生成Mipmap链
    if (mipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data);

    // 最终错误检查
    if (glGetError() == GL_NO_ERROR) {
        mTextureIfValid = true;
        glBindTexture(GL_TEXTURE_2D, 0); // 解绑
        return true;
    }
    else {
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
    int mipmapLevel = 1 + floor(log2(std::max(width, height)));
    if (nrChannels == 1) {
        glTexStorage2D(GL_TEXTURE_2D, mipmapLevel, GL_R, width, height);
        if (data) {
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_R, GL_UNSIGNED_BYTE, data);

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
    }
    else if (nrChannels == 3) {
        glTexStorage2D(GL_TEXTURE_2D, mipmapLevel, GL_RGB, width, height);
        if (data) {
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
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
    }
    else if (nrChannels == 4) {
        glTexStorage2D(GL_TEXTURE_2D, mipmapLevel, GL_RGBA, width, height);
        if (data) {
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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