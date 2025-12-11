#include "model.h"
#include "program.h"
#include "framebuffer.h"
#include "texture.h"
#include "camera.h"

#include "bilateralBlurPass.h"

namespace PbrGi {
    BilateralBlurPass::BilateralBlurPass(std::shared_ptr<Texture> aoTexture) {

    }

    BilateralBlurPass::~BilateralBlurPass() {

    }

    void BilateralBlurPass::render() {

    }

    std::shared_ptr<Texture> BilateralBlurPass::result() {
        return mBilateralBlurTextureY;
    }
}