#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "program.h"
#include "camera.h"
#include "texture.h"
#include <vector>

namespace PbrGi {
    class SkyBox {
        public:
            SkyBox(unsigned int width, unsigned int height, std::shared_ptr<camera> camera);
            void render();
            ~SkyBox();

            std::shared_ptr<Texture> getIBL() {
                return mIblTexture;
            }

            std::vector<float> getSH() {
                return mSH;
            }

        private:
            std::shared_ptr<Texture> mIblTexture;
            std::vector<float> mSH;
            std::shared_ptr<camera> mCamera;
            std::shared_ptr<Program> mProgram;
            unsigned int mWidth;
            unsigned int mHeight;
            unsigned int mVAO;
            unsigned int mVBO;
    };
}

#endif