#include "model.h"
#include "program.h"
#include "framebuffer.h"
#include "texture.h"
#include "camera.h"
#include "common.hpp"

#include "gtaoPass.h"

namespace PbrGi {

    GtaoPass::GtaoPass(std::shared_ptr<camera> camera, std::shared_ptr<Texture> depthTexture)
        : mCamera(camera)
        , mDepthTexture(depthTexture) {

        std::vector<std::string> textureNames = {"depthTexture"};
        mRenderProgram = std::make_shared<PbrGi::Program>(textureNames, ".\\shader\\gtao.vs", ".\\shader\\gtao.fs");

        mAmbientObscuranceTexture = std::make_shared<PbrGi::Texture>();
        mAmbientObscuranceTexture->init2DTexture(SCR_WIDTH, SCR_HEIGHT, GL_RGBA, false);

        mFramebuffer = std::make_shared<PbrGi::frameBuffer>(SCR_WIDTH, SCR_HEIGHT, false);
        std::vector<std::shared_ptr<PbrGi::Texture>> multiColorOutput;
        multiColorOutput.push_back(mAmbientObscuranceTexture);
        if (mFramebuffer->init(multiColorOutput)) {
            std::cout << "gtao framebuffer init success" << std::endl;
        } else {
            std::cout << "gtao framebuffer init failed" << std::endl;
        }

        mAmbientObscuranceTexture = mFramebuffer->getColorBuffer(0);

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

    GtaoPass::~GtaoPass() {

    }

    void GtaoPass::render() {
            mFramebuffer->setup();

            GLenum buffers[] = {GL_COLOR_ATTACHMENT1};
            glDrawBuffers(1, buffers);

            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            mRenderProgram->use();
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
            
            unsigned int textureId;
            if (mDepthTexture->getTextureId(textureId)) {
                mRenderProgram->setTexture2D("depthTexture", textureId);
            }

            mTextureModel->drawModel(mRenderProgram);

            mFramebuffer->unload();
    }

    std::shared_ptr<Texture> GtaoPass::result() {
        return mAmbientObscuranceTexture;
    }
}