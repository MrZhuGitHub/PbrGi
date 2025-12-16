#include "structurePass.h"
#include "common.hpp"

namespace PbrGi {
    StructurePass::StructurePass(std::shared_ptr<camera> camera)
        : mCamera(camera) {

        mRenderProgram = std::make_shared<PbrGi::Program>(std::vector<std::string>{}, ".\\shader\\structure.vs", ".\\shader\\structure.fs");
        mDepthTexture = std::make_shared<PbrGi::Texture>();

        mFramebuffer = std::make_shared<PbrGi::frameBuffer>(SCR_WIDTH, SCR_HEIGHT, true);

        if (mFramebuffer->init()) {
            std::cout << "structure framebuffer init success" << std::endl;
        } else {
            std::cout << "structure framebuffer init failed" << std::endl;
        }
        mDepthTexture = mFramebuffer->getDepthBuffer();

    }

    StructurePass::~StructurePass() {

    }
    
    void StructurePass::render(std::vector<std::shared_ptr<model>> models) {
        mFramebuffer->setup();

        GLenum buffers[] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, buffers);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mRenderProgram->use();
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        mRenderProgram->setViewMatrix(mCamera->getViewMatrix());
        mRenderProgram->setProjectionMatrix(mCamera->getProjectMatrix());

        mRenderProgram->setFloat(mCamera->near(), "near");
        mRenderProgram->setFloat(mCamera->far(), "far");

        for (auto& it : models) {
            it->drawModel(mRenderProgram);
        }

        mFramebuffer->unload();
    }
}