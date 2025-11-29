#ifndef _GAUSSIAN_BLUR_Pass_H_
#define _GAUSSIAN_BLUR_Pass_H_

#include "program.h"
#include "framebuffer.h"
#include "texture.h"

namespace PbrGi {
    class GaussianBlurPass {
        public:
            GaussianBlurPass();
            ~GaussianBlurPass();
            void render(std::shared_ptr<Texture> input, unsigned int size);
            std::shared_ptr<Texture> result() {
                return mGaussianBlurTextureY;
            }

        private:
            std::shared_ptr<Program> mRenderProgram;
            std::shared_ptr<frameBuffer> mFramebuffer;
            std::shared_ptr<Texture> mGaussianBlurTextureX;
            std::shared_ptr<Texture> mGaussianBlurTextureY;
    };
}

#endif