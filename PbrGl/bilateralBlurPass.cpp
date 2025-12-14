#include "model.h"
#include "program.h"
#include "framebuffer.h"
#include "texture.h"
#include "camera.h"
#include "common.hpp"

#include "bilateralBlurPass.h"

namespace PbrGi {
    BilateralBlurPass::BilateralBlurPass(std::shared_ptr<Texture> aoTexture, std::shared_ptr<Texture> depthTexture)
        : mAoTexture (aoTexture)
        , mDepthTexture(depthTexture) {

        std::vector<std::string> textureNames = {"depthTexture", "aoTexture"};
        mRenderProgram = std::make_shared<PbrGi::Program>(textureNames,  ".\\shader\\bilateralBlur.vs", ".\\shader\\bilateralBlur.fs");

        mBilateralBlurTextureX = std::make_shared<PbrGi::Texture>();
        mBilateralBlurTextureX->init2DTexture(SCR_WIDTH, SCR_HEIGHT, GL_RGBA, false);
        std::vector<std::shared_ptr<PbrGi::Texture>> multiColorOutputX;
        multiColorOutputX.push_back(mBilateralBlurTextureX);
        mFramebufferX = std::make_shared<PbrGi::frameBuffer>(SCR_WIDTH, SCR_HEIGHT, false);
        if (mFramebufferX->init(multiColorOutputX)) {
            std::cout << "BilateralBlur FramebufferX init success" << std::endl;
        } else {
            std::cout << "BilateralBlur FramebufferX init failed" << std::endl;
        }

        mBilateralBlurTextureY = std::make_shared<PbrGi::Texture>();
        mBilateralBlurTextureY->init2DTexture(SCR_WIDTH, SCR_HEIGHT, GL_RGBA, false);
        std::vector<std::shared_ptr<PbrGi::Texture>> multiColorOutputY;
        multiColorOutputY.push_back(mBilateralBlurTextureY);
        mFramebufferX = std::make_shared<PbrGi::frameBuffer>(SCR_WIDTH, SCR_HEIGHT, false);
        mFramebufferY = std::make_shared<PbrGi::frameBuffer>(SCR_WIDTH, SCR_HEIGHT, false);
        if (mFramebufferY->init(multiColorOutputY)) {
            std::cout << "BilateralBlur FramebufferY init success" << std::endl;
        } else {
            std::cout << "BilateralBlur FramebufferY init failed" << std::endl;
        }

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

    BilateralBlurPass::~BilateralBlurPass() {

    }

    std::vector<float> BilateralBlurPass::GaussianKernel(size_t const gaussianWidth, float const stdDev) {
        std::vector<float> outKernel;
        const size_t gaussianSampleCount = (gaussianWidth + 1u) / 2u;
        for (size_t i = 0; i < gaussianSampleCount; i++) {
            float const x = float(i);
            float const g = std::exp(-(x * x) / (2.0f * stdDev * stdDev));
            outKernel.push_back(g);
        }
        return outKernel;
    }

    void BilateralBlurPass::render() {

        {
            mFramebufferX->setup();

            GLenum buffers[] = {GL_COLOR_ATTACHMENT1};
            glDrawBuffers(1, buffers);

            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            mRenderProgram->use();
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
            
            unsigned int depthTextureId;
            if (mDepthTexture->getTextureId(depthTextureId)) {
                mRenderProgram->setTexture2D("depthTexture", depthTextureId);
            }

            unsigned int aoTextureId;
            if (mAoTexture->getTextureId(aoTextureId)) {
                mRenderProgram->setTexture2D("aoTexture", aoTextureId);
            }

            mTextureModel->drawModel(mRenderProgram);

            mFramebufferX->unload();
        }

        {
            mFramebufferY->setup();

            GLenum buffers[] = {GL_COLOR_ATTACHMENT1};
            glDrawBuffers(1, buffers);

            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            mRenderProgram->use();
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
            
            
            unsigned int depthTextureId, aoTextureId;
            if (mDepthTexture->getTextureId(depthTextureId)) {
                mRenderProgram->setTexture2D("depthTexture", depthTextureId);
            }

            if (mBilateralBlurTextureX->getTextureId(aoTextureId)) {
                mRenderProgram->setTexture2D("aoTexture", aoTextureId);
            }

            mTextureModel->drawModel(mRenderProgram);

            mFramebufferY->unload();
        }

    }

    std::shared_ptr<Texture> BilateralBlurPass::result() {
        return mBilateralBlurTextureY;
    }
}