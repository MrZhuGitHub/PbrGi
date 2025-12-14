#ifndef _BILATERAL_BLUR_PASS_
#define _BILATERAL_BLUR_PASS_

#include <memory>
#include <vector>

namespace PbrGi {

    class model;
    class customModel;
    class camera;
    class Texture;
    class Program;
    class frameBuffer;

    class BilateralBlurPass {
        public:
            BilateralBlurPass(std::shared_ptr<Texture> aoTexture, std::shared_ptr<Texture> depthTexture);
            ~BilateralBlurPass();
            void render();
            std::shared_ptr<Texture> result();

        private:
            std::vector<float> GaussianKernel(size_t const gaussianWidth, float const stdDev);

        private:
            std::shared_ptr<Program> mRenderProgram;
            std::shared_ptr<frameBuffer> mFramebufferX;
            std::shared_ptr<frameBuffer> mFramebufferY;
            std::shared_ptr<Texture> mBilateralBlurTextureX;
            std::shared_ptr<Texture> mBilateralBlurTextureY;
            std::shared_ptr<model> mTextureModel;
            std::shared_ptr<Texture> mAoTexture;
            std::shared_ptr<Texture> mDepthTexture;
    };
}

#endif