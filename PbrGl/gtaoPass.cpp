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
        mAmbientObscuranceTexture->init2DTexture(SCR_WIDTH, SCR_HEIGHT, GL_RGBA8, false);

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

            GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
            glDrawBuffers(2, buffers);

            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            mRenderProgram->use();
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

            mRenderProgram->setProperty(glm::vec2(SCR_WIDTH, SCR_HEIGHT), "resolution");
            mRenderProgram->setProperty(glm::inverse(mCamera->getProjectMatrix()), "invProjection");

            float radius = 0.3;
            const float projectionScale = std::min(
                    0.5f * mCamera->getProjectMatrix()[0][0] * SCR_WIDTH,
                    0.5f * mCamera->getProjectMatrix()[1][1] * SCR_HEIGHT);

            float projectionScaleRadius = projectionScale * radius;
            mRenderProgram->setProperty(projectionScaleRadius, "projectionScaleRadius");

            float stepsPerSlice = 3.0;
            mRenderProgram->setProperty(stepsPerSlice, "stepsPerSlice");

            glm::vec2 sliceCount = glm::vec2(4.0, 1.0/4.0);
            mRenderProgram->setProperty(sliceCount, "sliceCount");

            float thicknessHeuristic = 0.004;
            mRenderProgram->setProperty(thicknessHeuristic, "thicknessHeuristic");

            float invRadiusSquared = 1.0/(radius * radius);
            mRenderProgram->setProperty(invRadiusSquared, "invRadiusSquared");

            float power = 1.0;
            mRenderProgram->setProperty(power, "power");

            float invFarPlane = 1.0/mCamera->far();
            mRenderProgram->setProperty(invFarPlane, "invFarPlane");

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