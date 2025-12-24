#include "toneMapPass.h"

#include "model.h"
#include "framebuffer.h"
#include "texture.h"
#include "program.h"

#include "common.hpp"

namespace PbrGi {
    ToneMapPass::ToneMapPass(std::shared_ptr<Texture> mHdrTexture)
        : mHdrTexture(mHdrTexture) {

        std::vector<std::string> textureNames = {"hdrTexture"};

        mProgram = std::make_shared<PbrGi::Program>(textureNames, ".\\shader\\tonemap.vs", ".\\shader\\tonemap.fs");

        mFramebuffer = std::make_shared<PbrGi::frameBuffer>(SCR_WIDTH, SCR_HEIGHT);
        if (mFramebuffer->init()) {
            std::cout << "ToneMapPass framebuffer init success" << std::endl;
        } else {
            std::cout << "ToneMapPass framebuffer init failed" << std::endl;
        }

        mLdrTexture = mFramebuffer->getColorBuffer(0);

        std::vector<float> geometryData = {
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        };
        
        mTextureModel = std::make_shared<customModel>(geometryData, glm::vec3(1.0, 1.0, 1.0), 5);
        glm::mat4 trans1(1.0f);
        mTextureModel->addInstance(trans1);
    }

    void ToneMapPass::render() {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "TonemapPass");

        mFramebuffer->setup();

        GLenum buffers[] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, buffers);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mProgram->use();
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        
        unsigned int hdrTextureId;
        if (mHdrTexture->getTextureId(hdrTextureId)) {
            mProgram->setTexture2D("hdrTexture", hdrTextureId);
        }

        mProgram->setProperty(glm::vec2(SCR_WIDTH, SCR_HEIGHT), "resolution");

        mTextureModel->drawModel(mProgram);

        mFramebuffer->unload();

        mFramebuffer->blitToFrameBuffer(0, 0, 0);

        glPopDebugGroup();
    }

    ToneMapPass::~ToneMapPass() {

    }

}