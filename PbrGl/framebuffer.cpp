#include "framebuffer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace PbrGi {

    frameBuffer::frameBuffer(unsigned int width, unsigned int height, bool depthBuffer, bool mass, unsigned int samples)
        : width_(width)
        , height_(height)
        , mass_(mass)
        , samples_(samples)
        , initSuccess_(false)
        , depthBuffer_(depthBuffer) {

    }

    bool frameBuffer::init(std::vector<std::shared_ptr<Texture>> mutipleColorTextures, std::shared_ptr<Texture> depthTexture) {
        mMutipleColorTextures = mutipleColorTextures;
        if (mass_) {
            glGenFramebuffers(1, &frameBufferId_);
            glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId_);

            glGenTextures(1, &textureId_);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureId_);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples_, GL_RGBA16F, width_, height_, GL_TRUE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureId_, 0);

            glGenRenderbuffers(1, &renderBufferId_);
            glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId_);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples_, GL_DEPTH24_STENCIL8, width_, height_);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferId_);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                initSuccess_ = false;
                return false;
            }
            else {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                initSuccess_ = true;
                return true;
            }
        }
        else {
            glGenFramebuffers(1, &frameBufferId_);

            if (depthBuffer_) {
                glGenTextures(1, &textureId_);
                glBindTexture(GL_TEXTURE_2D, textureId_);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_FLOAT, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glGenerateMipmap(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, 0);

                if (depthTexture) {
                    depthTexture->getTextureId(depthbufferTextureId_);
                } else {
                    glGenTextures(1, &depthbufferTextureId_);
                    glBindTexture(GL_TEXTURE_2D, depthbufferTextureId_);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glGenerateMipmap(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, 0);
                }

                glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId_);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthbufferTextureId_, 0);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId_, 0);

                std::vector<GLenum> attachments;
                for (int i = 1; i <= mMutipleColorTextures.size(); i++) {
                    unsigned int textureId;
                    if (mMutipleColorTextures[i - 1]->getTextureId(textureId)) {
                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureId, 0);
                        attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
                    }
                }
                glDrawBuffers(attachments.size(), attachments.data());
            }

            if (!depthBuffer_) {

                glGenTextures(1, &textureId_);
                glBindTexture(GL_TEXTURE_2D, textureId_);
                //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width_, height_);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);

                glGenRenderbuffers(1, &renderBufferId_);
                glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId_);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_, height_);
                glBindRenderbuffer(GL_RENDERBUFFER, 0);

                glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId_);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId_, 0);

                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferId_);

                std::vector<GLenum> attachments;
                for (int i = 1; i <= mMutipleColorTextures.size(); i++) {
                    unsigned int textureId;
                    if (mMutipleColorTextures[i - 1]->getTextureId(textureId)) {
                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureId, 0);
                        attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
                    }
                }
                attachments.insert(attachments.begin(), GL_COLOR_ATTACHMENT0);
                glDrawBuffers(attachments.size(), attachments.data());
            }

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                std::cout << "framebuffer is not complete" << std::endl;
                exit(0);
                initSuccess_ = false;
                return false;
            }
            else {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                initSuccess_ = true;;
                return true;
            }
        }
    }

    frameBuffer::~frameBuffer() {
        glDeleteFramebuffers(1, &frameBufferId_);
        glDeleteTextures(1, &textureId_);
        glDeleteRenderbuffers(1, &renderBufferId_);
    }

    bool frameBuffer::setup() {
        if (!initSuccess_) {
            return initSuccess_;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId_);

        if (mass_) {
            glEnable(GL_MULTISAMPLE);
        }

        return initSuccess_;
    }

    bool frameBuffer::unload() {
        if (!initSuccess_) {
            return initSuccess_;
        }

        if (mass_) {
            glDisable(GL_MULTISAMPLE);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return initSuccess_;
    }

    bool frameBuffer::blitToFrameBuffer(unsigned int frameBuffer, unsigned int readAttachment, unsigned int writeAttachment) {
        if (!initSuccess_) {
            return initSuccess_;
        }

        glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferId_);
        glReadBuffer(GL_COLOR_ATTACHMENT0 + readAttachment);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
        GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0 + writeAttachment};
        glDrawBuffers(1, drawBufs);        

        glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        return initSuccess_;
    }

    bool frameBuffer::readPixels(unsigned int x, unsigned int y, float* pixels) {
        if (!initSuccess_) {
            return initSuccess_;
        }

        glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, pixels);

        return true;
    }

    bool frameBuffer::readPixels(unsigned int x, unsigned int y, unsigned int width, unsigned int height, float* pixels) {
        if (!initSuccess_) {
            return initSuccess_;
        }

        glReadPixels(x, y, width, height, GL_RGBA, GL_FLOAT, pixels);

        return true;
    }

    bool frameBuffer::readDepth(unsigned int x, unsigned int y, unsigned int width, unsigned int height, float* depth) {
        if (!initSuccess_) {
            return initSuccess_;
        }

        glReadPixels(x, y, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depth);

        return true;
    }

    bool frameBuffer::setFramebufferMipmapLevel(unsigned int mipmapLevel) {
        glGetError();

        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId_);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthbufferTextureId_, mipmapLevel);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId_, mipmapLevel);

        for (int i = 1; i <= mMutipleColorTextures.size(); i++) {
            unsigned int textureId;
            if (mMutipleColorTextures[i - 1]->getTextureId(textureId)) {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureId, mipmapLevel);
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (GL_NO_ERROR == glGetError()) {
            return true;
        } else {
            return false;
        }
    }

}