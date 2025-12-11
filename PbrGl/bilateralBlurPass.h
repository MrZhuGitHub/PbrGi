#ifndef _BILATERAL_BLUR_PASS_
#define _BILATERAL_BLUR_PASS_

#include <memory>

namespace PbrGi {

    class model;
    class customModel;
    class camera;
    class Texture;
    class Program;
    class frameBuffer;

    class BilateralBlurPass {
        public:
            BilateralBlurPass(std::shared_ptr<Texture> aoTexture);
            ~BilateralBlurPass();
            void render();
            std::shared_ptr<Texture> result();

        private:
            std::shared_ptr<Program> mRenderProgram;
            std::shared_ptr<frameBuffer> mFramebufferX;
            std::shared_ptr<frameBuffer> mFramebufferY;
            std::shared_ptr<Texture> mBilateralBlurTextureX;
            std::shared_ptr<Texture> mBilateralBlurTextureY;
            std::shared_ptr<model> mTextureModel;
            std::shared_ptr<Texture> mAoTexture;
    };
}

#endif