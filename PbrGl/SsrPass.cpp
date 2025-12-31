#include "SsrPass.h"

#include "model.h"
#include "framebuffer.h"
#include "texture.h"
#include "program.h"
#include "camera.h"
#include "common.hpp"

namespace PbrGi {
    SSrPass::SSrPass(std::shared_ptr<camera> camera, std::shared_ptr<Texture> depthTexture, std::shared_ptr<Texture> colorTexture, std::shared_ptr<Texture> normalTexture)
        : mDepthTexture(depthTexture) 
        , mColorTexture(colorTexture)
        , mNormalTexture(normalTexture)
        , mCamera(camera) {

        std::vector<std::string> textureNames = {"depthTexture", "colorTexture", "normalTexture"};
        mProgram = std::make_shared<PbrGi::Program>(textureNames, ".\\shader\\ssr.vs", ".\\shader\\ssr.fs");

        mRadianceTexture = std::make_shared<PbrGi::Texture>();
        mRadianceTexture->init2DTexture(SCR_WIDTH, SCR_HEIGHT, GL_RGBA16F, false);

        mFramebuffer = std::make_shared<PbrGi::frameBuffer>(SCR_WIDTH, SCR_HEIGHT, false);
        std::vector<std::shared_ptr<PbrGi::Texture>> colorAttachments;
        colorAttachments.push_back(mRadianceTexture);
        if (mFramebuffer->init(colorAttachments)) {
            std::cout << "SSrPass Framebuffer init success" << std::endl;
        } else {
            std::cout << "SSrPass Framebuffer init failed" << std::endl;
        }

        mResultTexture = mFramebuffer->getColorBuffer(0);

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

    SSrPass::~SSrPass() {

    }

    void SSrPass::render() {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "SSrPass");

        mFramebuffer->setup();

        GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, buffers);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mProgram->use();
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        unsigned int depthTextureId;
        if (mDepthTexture->getTextureId(depthTextureId)) {
            mProgram->setTexture2D("depthTexture", depthTextureId);
        }

        unsigned int colorTextureId;
        if (mColorTexture->getTextureId(colorTextureId)) {
            mProgram->setTexture2D("colorTexture", colorTextureId);
        }

        unsigned int normalTextureId;
        if (mNormalTexture->getTextureId(normalTextureId)) {
            mProgram->setTexture2D("normalTexture", normalTextureId);
        }

        mProgram->setProperty(glm::vec2(SCR_WIDTH, SCR_HEIGHT), "resolution");
        mProgram->setProperty(glm::inverse(mCamera->getProjectMatrix()), "invProjection");

        glm::mat4 uvFromClipMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.0)) * glm::scale(glm::mat4(1.0), glm::vec3(0.5, 0.5, 1.0));
        glm::mat4 uvFromViewMatrix = glm::scale(glm::mat4(1.0), glm::vec3(SCR_WIDTH, SCR_HEIGHT, 1.0)) * uvFromClipMatrix * mCamera->getProjectMatrix();
        mProgram->setProperty(uvFromViewMatrix, "uvFromViewMatrix");

        float near = mCamera->near();
        mProgram->setFloat("near", near);

        float far = mCamera->far();
        mProgram->setFloat("far", far);

        float vsZThickness = mCamera->near()/10.0;
        mProgram->setFloat("vsZThickness", vsZThickness);

        float ssrBias = 0.1;
        mProgram->setFloat("ssrBias", ssrBias);

        float maxRayTraceDistance = 20.0;
        mProgram->setFloat("maxRayTraceDistance", maxRayTraceDistance);

        float fadeRateDistance = 4.0;
        mProgram->setFloat("fadeRateDistance", fadeRateDistance);

        mTextureModel->drawModel(mProgram);

        mFramebuffer->unload();

        glPopDebugGroup();
    }
}
