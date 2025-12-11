#ifndef _GTAO_PASS_H_
#define _GTAO_PASS_H_

#include <memory>

namespace PbrGi {

    class model;
    class customModel;
    class camera;
    class Texture;
    class Program;
    class frameBuffer;

    class GtaoPass {
        public:
            GtaoPass(std::shared_ptr<camera> camera, std::shared_ptr<Texture> depthTexture);
            ~GtaoPass();
            void render();
            std::shared_ptr<Texture> result() {
                return mAmbientObscuranceTexture;
            }

        private:
            std::shared_ptr<Program> mRenderProgram;
            std::shared_ptr<frameBuffer> mFramebuffer;
            std::shared_ptr<Texture> mAmbientObscuranceTexture;
            std::shared_ptr<Texture> mDepthTexture;
            std::shared_ptr<camera> mCamera;
    };
}

#endif