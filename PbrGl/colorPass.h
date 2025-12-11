#ifndef _COLOR_PASS_H_
#define _COLOR_PASS_H_

#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace PbrGi {
    class frameBuffer;
    class Texture;
    class camera;
    class model;
    class SkyBox;
    class customModel;
    class Program;
    class ColorPass {
        public:
            ColorPass(std::shared_ptr<camera> camera1, std::shared_ptr<SkyBox> skybox, 
                        std::shared_ptr<Texture> bilateralBlurAoTexture = nullptr,
                        std::shared_ptr<Texture> gaussianBlurDepthTexture = nullptr, 
                        std::shared_ptr<camera> lightCamera = nullptr);
            void render(std::vector<std::shared_ptr<model>> models, bool skybox);
            ~ColorPass();

        private:
            std::shared_ptr<frameBuffer> mFrameBuffer;
            std::shared_ptr<Texture> mIblTexture;
            std::shared_ptr<Texture> mDfgTexture;
            std::shared_ptr<Program> mProgram;
            std::shared_ptr<camera> mCamera;
            std::vector<float> mSH;
            std::shared_ptr<Texture> mGaussianBlurDepthTexture;
            std::shared_ptr<SkyBox> mSkybox;
            std::shared_ptr<camera> mLightCamera;
            std::shared_ptr<Texture> mBilateralBlurAoTexture;
    };
}

#endif