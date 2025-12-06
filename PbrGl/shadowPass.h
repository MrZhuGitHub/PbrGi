#ifndef _SHADOW_PASS_H_
#define _SHADOW_PASS_H_

#include "model.h"
#include "program.h"
#include "framebuffer.h"
#include "texture.h"
#include "camera.h"

namespace PbrGi {
    class ShadowPass {
        public:
            ShadowPass(std::shared_ptr<camera> lightCamera);
            ~ShadowPass();
            void render(std::vector<std::shared_ptr<model>> models);
            std::shared_ptr<Texture> result() {
                return mDepthTexture;
            }

        private:
            std::shared_ptr<Program> mRenderProgram;
            std::shared_ptr<frameBuffer> mFramebuffer;
            std::shared_ptr<camera> mLightPosition;
            std::shared_ptr<Texture> mDepthTexture;
    };
}

#endif