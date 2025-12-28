#ifndef _STRUCTURE_PASS_H_
#define _STRUCTURE_PASS_H_

#include "model.h"
#include "program.h"
#include "framebuffer.h"
#include "texture.h"
#include "camera.h"

namespace PbrGi {
    class StructurePass {
        public:
            StructurePass(std::shared_ptr<camera> camera);
            ~StructurePass();
            void render(std::vector<std::shared_ptr<model>> models);
            std::shared_ptr<Texture> result() {
                return mDepthTexture;
            }

            std::shared_ptr<Texture> normal() {
                return mNormalTexture;
            }

        private:
            std::shared_ptr<Program> mRenderProgram;
            std::shared_ptr<frameBuffer> mFramebuffer;
            std::shared_ptr<camera> mCamera;
            std::shared_ptr<Texture> mDepthTexture;
            std::shared_ptr<Texture> mNormalTexture;
    };
}

#endif