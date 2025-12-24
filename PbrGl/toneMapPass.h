#ifndef _TONEMAP_PASS_
#define _TONEMAP_PASS_

#include <memory>

namespace PbrGi {

    class model;
    class customModel;
    class Texture;
    class Program;
    class frameBuffer;

    class ToneMapPass {
        public:
            ToneMapPass(std::shared_ptr<Texture> mHdrTexture);
            ~ToneMapPass();
            void render();
            std::shared_ptr<Texture> result() {
                return mLdrTexture;
            }

        private:
            std::shared_ptr<Program> mProgram;
            std::shared_ptr<frameBuffer> mFramebuffer;
            std::shared_ptr<Texture> mHdrTexture;
            std::shared_ptr<Texture> mLdrTexture;
            std::shared_ptr<model> mTextureModel;
    };
}

#endif