#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "program.h"
#include "camera.h"
#include "texture.h"

namespace PbrGi {
    class SkyBox {
        public:
            SkyBox(unsigned int width, unsigned int height, std::shared_ptr<camera> camera, std::shared_ptr<Texture> texture);
            void render();
            ~SkyBox();

        private:
            std::shared_ptr<Texture> mTexture;
            std::shared_ptr<camera> mCamera;
            std::shared_ptr<Program> mProgram;
            unsigned int mWidth;
            unsigned int mHeight;
            unsigned int mVAO;
            unsigned int mVBO;
    };
}

#endif