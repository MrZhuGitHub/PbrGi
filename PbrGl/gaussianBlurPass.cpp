#include "gaussianBlurPass.h"
#include "common.hpp"

namespace PbrGi {
    GaussianBlurPass::GaussianBlurPass(std::shared_ptr<Texture> depth)
        : mDepth(depth) {

        std::vector<std::string> textureNames = {"depthTexture"};
        mRenderProgram = std::make_shared<Program>(textureNames,  ".\\shader\\gaussianBlurVertex.glsl", ".\\shader\\gaussianBlurFragment.glsl");

        {
            mGaussianBlurTextureX = std::make_shared<PbrGi::Texture>();
            mGaussianBlurTextureX->init2DTexture(SCR_WIDTH, SCR_HEIGHT, GL_RGB16F, false);
            mFramebufferX = std::make_shared<PbrGi::frameBuffer>(SCR_WIDTH, SCR_HEIGHT, true);

            std::vector<std::shared_ptr<PbrGi::Texture>> multiColorOutput;
            multiColorOutput.push_back(mGaussianBlurTextureX);
            if (mFramebufferX->init(multiColorOutput)) {
                std::cout << "shadow GaussianBlur framebufferx init success" << std::endl;
            } else {
                std::cout << "shadow GaussianBlur framebufferx init failed" << std::endl;
            }
        }

        {
            mGaussianBlurTextureY = std::make_shared<PbrGi::Texture>();
            mGaussianBlurTextureY->init2DTexture(SCR_WIDTH, SCR_HEIGHT, GL_RGB16F, false);
            mFramebufferY = std::make_shared<PbrGi::frameBuffer>(SCR_WIDTH, SCR_HEIGHT, true);

            std::vector<std::shared_ptr<PbrGi::Texture>> multiColorOutput;
            multiColorOutput.push_back(mGaussianBlurTextureY);
            if (mFramebufferY->init(multiColorOutput)) {
                std::cout << "shadow GaussianBlur framebufferY init success" << std::endl;
            } else {
                std::cout << "shadow GaussianBlur framebufferY init failed" << std::endl;
            }
        }

        std::vector<float> geometryData = {
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        };
        
        mTextureModel = std::make_shared<customModel>(geometryData);
        glm::mat4 trans1(1.0f);
        mTextureModel->addInstance(trans1);
    }

    void GaussianBlurPass::render(unsigned int size) {
        {
            mFramebufferX->setup();

            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            mRenderProgram->use();
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

            unsigned int textureId;
            if (mDepth->getTextureId(textureId)) {
                mRenderProgram->setTexture2D("depthTexture", textureId);
            }

            mTextureModel->drawModel(mRenderProgram);

            mFramebufferX->unload();
        }

        {
            mFramebufferY->setup();

            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            mRenderProgram->use();
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

            unsigned int textureId;
            if (mGaussianBlurTextureX->getTextureId(textureId)) {
                mRenderProgram->setTexture2D("depthTexture", textureId);
            }

            mTextureModel->drawModel(mRenderProgram);

            mFramebufferY->unload();
        }

    }

    GaussianBlurPass::~GaussianBlurPass() {

    }
}

