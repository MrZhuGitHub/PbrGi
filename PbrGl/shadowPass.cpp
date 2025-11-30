#include "shadowPass.h"
#include "common.hpp"

namespace PbrGi {
    ShadowPass::ShadowPass(glm::vec3 lightPosition) {
        mLightPosition = std::make_shared<PbrGi::camera>(SCR_WIDTH, SCR_HEIGHT, 0.5f, 5000.0f, lightPosition);
        mRenderProgram = std::make_shared<PbrGi::Program>(std::vector<std::string>{}, ".\\shader\\shadowDepthVertex.glsl", ".\\shader\\shadowDepthFragment.glsl");
        mDepthTexture = std::make_shared<PbrGi::Texture>();
        mDepthTexture->init2DTexture(SCR_WIDTH, SCR_HEIGHT, GL_RGB16F, false);
        mFramebuffer = std::make_shared<PbrGi::frameBuffer>(SCR_WIDTH, SCR_HEIGHT, true);

        std::vector<std::shared_ptr<PbrGi::Texture>> multiColorOutput;
        multiColorOutput.push_back(mDepthTexture);
        if (mFramebuffer->init(multiColorOutput)) {
            std::cout << "shadow depth framebuffer init success" << std::endl;
        } else {
            std::cout << "shadow depth framebuffer init failed" << std::endl;
        }
    }

    ShadowPass::~ShadowPass() {

    }
    
    void ShadowPass::render(std::vector<std::shared_ptr<model>> models) {
        mFramebuffer->setup();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mRenderProgram->use();
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        mRenderProgram->setViewMatrix(mLightPosition->getViewMatrix());
        mRenderProgram->setProjectionMatrix(mLightPosition->getProjectMatrix());

        for (auto& it : models) {
            it->drawModel(mRenderProgram);
        }

        mFramebuffer->unload();
    }
}