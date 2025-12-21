#include "shadowPass.h"
#include "common.hpp"

namespace PbrGi {
    ShadowPass::ShadowPass(std::shared_ptr<camera> lightCamera)
        : mLightPosition(lightCamera) {

        mRenderProgram = std::make_shared<PbrGi::Program>(std::vector<std::string>{}, ".\\shader\\shadowDepth.vs", ".\\shader\\shadowDepth.fs");
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
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "ShadowPass");

        mFramebuffer->setup();

        GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, buffers);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mRenderProgram->use();
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        mRenderProgram->setViewMatrix(mLightPosition->getViewMatrix());
        mRenderProgram->setProjectionMatrix(mLightPosition->getProjectMatrix());

        mRenderProgram->setFloat(mLightPosition->near(), "near");
        mRenderProgram->setFloat(mLightPosition->far(), "far");
        float vsmExponent = 5.54f;
        mRenderProgram->setFloat(vsmExponent, "vsmExponent");

        for (auto& it : models) {
            it->drawModel(mRenderProgram);
        }

        mFramebuffer->unload();
        glPopDebugGroup();

    }
}