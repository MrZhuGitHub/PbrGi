#ifndef _FRAME_BUFFER_H_
#define _FRAME_BUFFER_H_

#include <vector>
#include <memory>

#include "texture.h"

namespace PbrGi {

    enum PIXEL {
        R = 0,
        G,
        B,
        A,
    };

    class frameBuffer {
    public:
        frameBuffer(unsigned int width, unsigned int height, bool depthBuffer = false, bool mass = false, unsigned int samples = 4);

        ~frameBuffer();

        bool init(std::vector<std::shared_ptr<Texture>> mutipleColorTextures = {});

        bool setup();

        bool unload();

        bool blitToFrameBuffer(unsigned int frameBuffer);

        std::shared_ptr<Texture> getColorBuffer(unsigned int index = 0) const {
            if (0 == index) {
                return std::make_shared<Texture>(textureId_);
            }
            else {
                if (index <= mMutipleColorTextures.size()) {
                    return mMutipleColorTextures[index - 1];
                }
            }
            return nullptr;
        }

        unsigned int getFrameBuffer() const {
            return frameBufferId_;
        }

        unsigned int getRenderBuffer() const {
            return renderBufferId_;
        }

        std::shared_ptr<Texture> getDepthBuffer() const {
            return std::make_shared<Texture>(depthbufferTextureId_);
        }

        bool readPixels(unsigned int x, unsigned int y, float* pixels);

        bool readPixels(unsigned int x, unsigned int y, unsigned int width, unsigned int height, float* pixels);

        bool readDepth(unsigned int x, unsigned int y, unsigned int width, unsigned int height, float* depth);

    private:
        unsigned int textureId_;
        unsigned int depthbufferTextureId_;
        unsigned int frameBufferId_;
        unsigned int renderBufferId_;
        unsigned int width_;
        unsigned int height_;
        bool mass_;
        unsigned int samples_;
        bool initSuccess_;
        bool depthBuffer_;
        std::vector<std::shared_ptr<Texture>> mMutipleColorTextures;
    };

}

#endif