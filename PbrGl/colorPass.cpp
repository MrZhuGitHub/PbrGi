#include "colorPass.h"

namespace PbrGi {
    ColorPass::ColorPass(std::shared_ptr<camera> camera, std::shared_ptr<SkyBox> skybox, std::shared_ptr<Texture> gaussianBlurDepthTexture)
        : mCamera (camera)
        , mSkybox(skybox)
        , mGaussianBlurDepthTexture(gaussianBlurDepthTexture) {

        mSH = mSkybox->getSH();
        mIblTexture = mSkybox->getIBL();

        std::vector<std::string> textureNames = {"baseColorTexture", 
                                                "sampler0_iblDFG", 
                                                "sampler0_iblSpecular", 
                                                "roughnessTexture", 
                                                "normalTexture", 
                                                "metalnessTexture", 
                                                "emissionTexture",
                                                "vsmDepthTexture"};

        mProgram = std::make_shared<PbrGi::Program>(textureNames, ".\\shader\\pbrVertex.glsl", ".\\shader\\pbrFragment.glsl");

        mFrameBuffer = std::make_shared<PbrGi::frameBuffer>(SCR_WIDTH, SCR_HEIGHT, false, true, 8);
        mFrameBuffer->init();

        mDfgTexture = std::make_shared<PbrGi::Texture>();
        mDfgTexture->init2DTextureHDR(".\\asset\\dfg\\dfg.hdr", true);

        mProgram->use();
        unsigned int id;
        mIblTexture->getTextureId(id);
        mProgram->setTextureCube("sampler0_iblSpecular", id);

        unsigned int id1;
        mDfgTexture->getTextureId(id1);
        mProgram->setTexture2D("sampler0_iblDFG", id1);

        mProgram->setFloat("sampler0_iblSpecular_mipmapLevel", 4);

        mProgram->setVec3Float("iblSH", mSH.data(), 9);
    }

    void ColorPass::render(std::vector<std::shared_ptr<model>> models, bool skybox) {
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

        for (auto& model : models) {
            model->drawModel(mProgram);
        }

        mFrameBuffer->unload();
        mFrameBuffer->blitToFrameBuffer(0);
    }

    ColorPass::~ColorPass() {

    }
}