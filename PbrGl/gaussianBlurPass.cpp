#include "gaussianBlurPass.h"
#include "common.hpp"


#include "program.h"
#include "framebuffer.h"
#include "texture.h"
#include "model.h"

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
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        };
        
        mTextureModel = std::make_shared<customModel>(geometryData, glm::vec3(1.0, 1.0, 1.0), 5);
        glm::mat4 trans1(1.0f);
        mTextureModel->addInstance(trans1);

    }

    std::vector<glm::vec2> GaussianBlurPass::GaussianBlurKernel(float blurWidth) {
        std::vector<glm::vec2> kernels;

        float sigma = (blurWidth + 1.0f) / 6.0f;
        size_t kernelWidth = std::ceil((blurWidth - 5.0f) / 4.0f);
        kernelWidth = kernelWidth * 4 + 5;

        float alpha = 1.0f / (2.0f * sigma * sigma);

        glm::vec2 kernel0;
        size_t m = (kernelWidth - 1) / 4 + 1;
        m = m < 64 ? m : 64;
        kernel0.x = 1.0;
        kernel0.y = 0.0;
        float totalWeight = kernel0.x;
        kernels.push_back(kernel0);

        for (size_t i = 1; i < m; i++) {
            glm::vec2 kernel;
            float x0 = float(i * 2 - 1);
            float x1 = float(i * 2);
            float k0 = std::exp(-alpha * x0 * x0);
            float k1 = std::exp(-alpha * x1 * x1);

            float k = k0 + k1;
            float o = k1 / k;
            kernel.x = k;
            kernel.y = o;
            totalWeight += (k0 + k1) * 2.0f;
            kernels.push_back(kernel);
        }
        for (size_t i = 0; i < m; i++) {
            kernels[i].x *= 1.0f / totalWeight;
        }

        return kernels;
    }

    void GaussianBlurPass::render(float blurWidth) {

        {
            mFramebufferX->setup();

            GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
            glDrawBuffers(2, buffers);

            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            mRenderProgram->use();
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

            std::vector<glm::vec2> kernels = GaussianBlurKernel(blurWidth);
            mRenderProgram->setVec2Float("kernel", (float*)kernels.data(), kernels.size());
            mRenderProgram->setInt("count", kernels.size());

            glm::vec2 resolution;
            resolution.x = (float)SCR_WIDTH;
            resolution.y = (float)SCR_HEIGHT;
            mRenderProgram->setProperty(resolution, "resolution");

            glm::vec2 axis;
            axis.x = 1.0f/(float)SCR_WIDTH;
            axis.y = 0.0f;
            mRenderProgram->setProperty(axis, "axis");
            
            unsigned int textureId;
            if (mDepth->getTextureId(textureId)) {
                mRenderProgram->setTexture2D("depthTexture", textureId);
            }

            mTextureModel->drawModel(mRenderProgram);

            mFramebufferX->unload();
        }

        {
            mFramebufferY->setup();

            GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
            glDrawBuffers(2, buffers);

            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            mRenderProgram->use();
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

            std::vector<glm::vec2> kernels = GaussianBlurKernel(blurWidth);
            mRenderProgram->setVec2Float("kernel", (float*)kernels.data(), kernels.size());
            mRenderProgram->setInt("count", kernels.size());

            glm::vec2 resolution;
            resolution.x = (float)SCR_WIDTH;
            resolution.y = (float)SCR_HEIGHT;
            mRenderProgram->setProperty(resolution, "resolution");

            glm::vec2 axis;
            axis.x = 0.0f;
            axis.y = 1.0f/(float)SCR_HEIGHT;
            mRenderProgram->setProperty(axis, "axis");

            unsigned int textureId;
            if (mGaussianBlurTextureX->getTextureId(textureId)) {
                mRenderProgram->setTexture2D("depthTexture", textureId);
            }

            mTextureModel->drawModel(mRenderProgram);

            mFramebufferY->unload();
        }

    }

    std::shared_ptr<Texture> GaussianBlurPass::result() {
        return mGaussianBlurTextureY;
    }

    GaussianBlurPass::~GaussianBlurPass() {

    }
}

