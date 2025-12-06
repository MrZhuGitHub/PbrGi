#ifndef _GAUSSIAN_BLUR_Pass_H_
#define _GAUSSIAN_BLUR_Pass_H_

#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace PbrGi {

    class Program;
    class Texture;
    class frameBuffer;
    class model;
    class customModel;

    class GaussianBlurPass {
        public:
            GaussianBlurPass(std::shared_ptr<Texture> depth);
            ~GaussianBlurPass();
            void render(float blurWidth);
            std::shared_ptr<Texture> result();

        private:
            std::vector<glm::vec2> GaussianBlurKernel(float blurWidth);

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