#ifndef _GAUSSIAN_BLUR_Pass_H_
#define _GAUSSIAN_BLUR_Pass_H_

#include "program.h"
#include "framebuffer.h"
#include "texture.h"
#include "model.h"

namespace PbrGi {
    class GaussianBlurPass {
        public:
            GaussianBlurPass(std::shared_ptr<Texture> depth);
            ~GaussianBlurPass();
            void render(unsigned int size);
            std::shared_ptr<Texture> result() {
                return mGaussianBlurTextureY;
            }

        private:
            std::shared_ptr<Program> mRenderProgram;
            std::shared_ptr<frameBuffer> mFramebufferX;
            std::shared_ptr<frameBuffer> mFramebufferY;
            std::shared_ptr<Texture> mGaussianBlurTextureX;
            std::shared_ptr<Texture> mGaussianBlurTextureY;
            std::shared_ptr<model> mTextureModel;
            std::shared_ptr<Texture> mDepth;
    };
}

#endif