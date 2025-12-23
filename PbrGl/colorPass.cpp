#include "colorPass.h"

#include "program.h"
#include "framebuffer.h"
#include "texture.h"
#include "model.h"
#include "camera.h"
#include "common.hpp"
#include "skybox.h"

namespace PbrGi {
    ColorPass::ColorPass(std::shared_ptr<camera> camera1, std::shared_ptr<SkyBox> skybox,
                        std::shared_ptr<Texture> bilateralBlurAoTexture,
                        std::shared_ptr<Texture> gaussianBlurDepthTexture,
                        std::shared_ptr<camera> lightCamera)
        : mCamera (camera1)
        , mSkybox(skybox)
        , mGaussianBlurDepthTexture(gaussianBlurDepthTexture)
        , mLightCamera(lightCamera)
        , mBilateralBlurAoTexture(bilateralBlurAoTexture) {

        mSH = mSkybox->getSH();
        mIblTexture = mSkybox->getIBL();

        std::vector<std::string> textureNames = {"baseColorTexture", 
                                                "sampler0_iblDFG", 
                                                "sampler0_iblSpecular", 
                                                "roughnessTexture", 
                                                "normalTexture", 
                                                "metalnessTexture", 
                                                "emissionTexture",
                                                "shadowMapTexture",
                                                "aoTexture"};

        mProgram = std::make_shared<PbrGi::Program>(textureNames, ".\\shader\\pbr.vs", ".\\shader\\pbr.fs");

        mFrameBuffer = std::make_shared<PbrGi::frameBuffer>(SCR_WIDTH, SCR_HEIGHT, false, true, 8);
        if (mFrameBuffer->init()) {
            std::cout << "colorPass framebuffer init success" << std::endl;
        } else {
            std::cout << "colorPass framebuffer init failed" << std::endl;
        }

        mFrameBufferResolve = std::make_shared<PbrGi::frameBuffer>(SCR_WIDTH, SCR_HEIGHT);
        if (mFrameBufferResolve->init()) {
            std::cout << "colorPass resolve framebuffer init success" << std::endl;
        } else {
            std::cout << "colorPass resolve framebuffer init failed" << std::endl;
        }

        mDfgTexture = std::make_shared<PbrGi::Texture>();
        mDfgTexture->init2DTextureHDR(".\\asset\\dfg\\dfg.hdr", true);

        mProgram->use();

        mProgram->setFloat("sampler0_iblSpecular_mipmapLevel", 4);

        mProgram->setVec3Float("iblSH", mSH.data(), 9);
    }

    void ColorPass::render(std::vector<std::shared_ptr<model>> models, bool skybox) {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "ColorPass");

        mFrameBuffer->setup();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (skybox && mSkybox) {
            mSkybox->render();
        }
        
        mProgram->use();
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        mProgram->setViewMatrix(mCamera->getViewMatrix());
        mProgram->setProjectionMatrix(mCamera->getProjectMatrix());
        mProgram->setProperty(mCamera->getCameraPosition(), "cameraPosition");

        unsigned int iblTextureId;
        mIblTexture->getTextureId(iblTextureId);
        mProgram->setTextureCube("sampler0_iblSpecular", iblTextureId);

        unsigned int dfgTextureId;
        mDfgTexture->getTextureId(dfgTextureId);
        mProgram->setTexture2D("sampler0_iblDFG", dfgTextureId);

        if (mBilateralBlurAoTexture) {
            unsigned int aoTextureId;
            if (mBilateralBlurAoTexture->getTextureId(aoTextureId)) {
                mProgram->setTexture2D("aoTexture", aoTextureId);
                mProgram->setBool("aoTextureExist", true);
            } else {
                mProgram->setBool("aoTextureExist", false);
            }
        } else {
            mProgram->setBool("aoTextureExist", false);
        }

        mProgram->setProperty(glm::vec2(SCR_WIDTH, SCR_HEIGHT), "resolution");

        if (mGaussianBlurDepthTexture && mLightCamera) { 
            mProgram->setBool("shadowMapExist", true);
            unsigned int shadowMapTexture;
            if (mGaussianBlurDepthTexture->getTextureId(shadowMapTexture)) {
                mProgram->setTexture2D("shadowMapTexture", shadowMapTexture);
            } else {
                mProgram->setBool("shadowMapExist", false);
            }

            mProgram->setProperty(mLightCamera->getViewMatrix(), "lightCameraViewMatrix");
            mProgram->setProperty(mLightCamera->getProjectMatrix(), "lightCameraProjectionMatrix");
            mProgram->setFloat(mLightCamera->near(), "near");
            mProgram->setFloat(mLightCamera->far(), "far");
            float vsmExponent = 5.54f;
            mProgram->setFloat(vsmExponent, "vsmExponent");

            float vsmDepthScale = 0.0277f;
            mProgram->setFloat(vsmDepthScale, "vsmDepthScale");

            float vsmLightBleedReduction = 0.15;
            mProgram->setFloat(vsmLightBleedReduction, "vsmLightBleedReduction");

        } else {
            mProgram->setBool("shadowMapExist", false);
        }

        for (auto& model : models) {
            model->drawModel(mProgram);
        }
        
        mFrameBuffer->unload();


        mFrameBuffer->blitToFrameBuffer(mFrameBufferResolve->getFrameBuffer());

        mFrameBufferResolve->blitToFrameBuffer(0);
        glPopDebugGroup();

    }

    ColorPass::~ColorPass() {

    }
}