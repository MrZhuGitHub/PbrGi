#include "SsrPass.h"

#include "model.h"
#include "framebuffer.h"
#include "texture.h"
#include "program.h"

namespace PbrGi {
    SSrPass::SSrPass(std::shared_ptr<Texture> depthTexture, std::shared_ptr<Texture> colorTexture)
        : mDepthTexture(depthTexture) 
        , mColorTexture(colorTexture) {

    }

    SSrPass::~SSrPass() {

    }

    void SSrPass::render() {

    }
}
