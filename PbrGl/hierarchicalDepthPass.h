#ifndef _HIERARCHICAL_DEPTH_PASS_H_
#define _HIERARCHICAL_DEPTH_PASS_H_

#include <memory>

namespace PbrGi {

    class model;
    class customModel;
    class camera;
    class Texture;
    class Program;
    class frameBuffer;

    class HierarchicalDepthPass {
        public:
            HierarchicalDepthPass(std::shared_ptr<Texture> depthTexture);
            ~HierarchicalDepthPass();
            void render();
            std::shared_ptr<Texture> result() {
                return mHierarchicalDepthTexture;
            }

        private:
            std::shared_ptr<Program> mProgram;
            std::shared_ptr<frameBuffer> mFramebuffer;
            std::shared_ptr<Texture> mDepthTexture;
            std::shared_ptr<Texture> mHierarchicalDepthTexture;
            std::shared_ptr<model> mTextureModel;
    };
}

#endif