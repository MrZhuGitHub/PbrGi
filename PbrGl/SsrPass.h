#ifndef _SSR_PASS_
#define _SSR_PASS_

#include <memory>

namespace PbrGi {

    class model;
    class customModel;
    class Texture;
    class Program;
    class frameBuffer;

    class SSrPass {
        public:
            SSrPass(std::shared_ptr<Texture> depthTexture, std::shared_ptr<Texture> colorTexture, std::shared_ptr<Texture> normalTexture);
            ~SSrPass();
            void render();
            std::shared_ptr<Texture> result() {
                return mResultTexture;
            }

        private:
            std::shared_ptr<Program> mRenderProgram;
            std::shared_ptr<frameBuffer> mFramebuffer;
            std::shared_ptr<Texture> mResultTexture;
            std::shared_ptr<Texture> mDepthTexture;
            std::shared_ptr<Texture> mColorTexture;
            std::shared_ptr<model> mTextureModel;
            std::shared_ptr<Texture> mNormalTexture;
    };
}

#endif