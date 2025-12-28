#include "hierarchicalDepthPass.h"

#include "model.h"
#include "program.h"
#include "framebuffer.h"
#include "texture.h"
#include "camera.h"
#include "common.hpp"

namespace PbrGi {
    HierarchicalDepthPass::HierarchicalDepthPass(std::shared_ptr<Texture> depthTexture)
        : mDepthTexture(depthTexture) {

        std::vector<std::string> textureNames = {"u_depthBuffer"};
        mProgram = std::make_shared<PbrGi::Program>(textureNames, ".\\shader\\depthMipmap.vs", ".\\shader\\depthMipmap.fs");

        mFramebuffer = std::make_shared<PbrGi::frameBuffer>(SCR_WIDTH, SCR_HEIGHT, true);
        std::vector<std::shared_ptr<PbrGi::Texture>> emptyColorTexture;
        if (mFramebuffer->init(emptyColorTexture, mDepthTexture)) {
            std::cout << "HierarchicalDepthPass Framebuffer init success" << std::endl;
        } else {
            std::cout << "HierarchicalDepthPass Framebuffer init failed" << std::endl;
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

        mHierarchicalDepthTexture = mFramebuffer->getDepthBuffer();
    }

    HierarchicalDepthPass::~HierarchicalDepthPass() {

    }

    void HierarchicalDepthPass::render() {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "HierarchicalDepthPass");

        mProgram->use();

        glDepthMask(true);

        int currentWidth = SCR_WIDTH;
        int currentHeight = SCR_HEIGHT;
        int numLevels = 1 + (int)floorf(log2f(fminf(currentWidth, currentHeight)));

        unsigned int depthTextureId;
        if (mDepthTexture->getTextureId(depthTextureId)) {
            mProgram->setTexture2D("u_depthBuffer", depthTextureId);
        }

        for (int i = 1; i < numLevels; i++) {
            mFramebuffer->setFramebufferMipmapLevel(i);
            mFramebuffer->setup();

            mProgram->setInt("u_previousLevel", i - 1);

            mProgram->setProperty(glm::ivec2(currentWidth, currentHeight), "u_previousLevelDimensions"); 
        
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glViewport(0, 0, currentWidth, currentHeight);

            mTextureModel->drawModel(mProgram);

            currentWidth = currentWidth / 2;
            currentHeight = currentHeight / 2;

            mFramebuffer->unload();
        }

        glPopDebugGroup();
    }
}