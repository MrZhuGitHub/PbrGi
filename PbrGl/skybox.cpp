#include "skybox.h"

#include <algorithm>

    static std::vector<std::string> prefilterIbl = {
                ".\\asset\\environment\\lightroom\\lightroom_14b\\m0_px.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m0_nx.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m0_py.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m0_ny.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m0_pz.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m0_nz.hdr",
                ".\\asset\\environment\\lightroom\\lightroom_14b\\m1_px.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m1_nx.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m1_py.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m1_ny.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m1_pz.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m1_nz.hdr",
                ".\\asset\\environment\\lightroom\\lightroom_14b\\m2_px.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m2_nx.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m2_py.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m2_ny.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m2_pz.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m2_nz.hdr",
                ".\\asset\\environment\\lightroom\\lightroom_14b\\m3_px.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m3_nx.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m3_py.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m3_ny.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m3_pz.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m3_nz.hdr",
                ".\\asset\\environment\\lightroom\\lightroom_14b\\m4_px.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m4_nx.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m4_py.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m4_ny.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m4_pz.hdr",
        ".\\asset\\environment\\lightroom\\lightroom_14b\\m4_nz.hdr",
    };

    static float sh[] = {
         0.785786807537079,  0.785786807537079,  0.785786807537079,
         0.402588516473770,  0.402588516473770,  0.402588516473770,
         0.460519373416901,  0.460519373416901,  0.460519373416901,
         0.084180898964405,  0.084180898964405,  0.084180898964405,
         0.058341909199953,  0.058341909199953,  0.058341909199953,
         0.204982891678810,  0.204982891678810,  0.204982891678810,
         0.092737942934036,  0.092737942934036,  0.092737942934036,
         -0.091809459030628, -0.091809459030628, -0.091809459030628,
         -0.006748970132321, -0.006748970132321, -0.006748970132321
    };

namespace PbrGi {

    SkyBox::SkyBox(unsigned int width, unsigned int height, std::shared_ptr<camera> camera) 
        : mCamera(camera)
        , mWidth(width)
        , mHeight(height) {

        mIblTexture = std::make_shared<PbrGi::Texture>();
        unsigned int prefilterIblMimmap = 5;
        mIblTexture->TestInitCubeTextureHDR(prefilterIbl, prefilterIblMimmap);

        mSH.resize(27);
        std::copy(sh, sh + 27, mSH.begin());

        std::vector<std::string> textureNames = { "skybox" };
        mProgram = std::make_shared<PbrGi::Program>(textureNames, ".\\shader\\skyboxVertex.glsl", ".\\shader\\skyboxFragment.glsl");
                                                       
        float cube[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };

        glGenBuffers(1, &mVBO);
        glGenVertexArrays(1, &mVAO);

        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);

        glBufferData(GL_ARRAY_BUFFER, 6*6*8*sizeof(GL_FLOAT), cube, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GL_FLOAT), (void*)0);

        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GL_FLOAT), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GL_FLOAT), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void SkyBox::render() {
        mProgram->use();

        glViewport(0, 0, mWidth, mHeight);

        //Mutiply glm::vec3(0.5f, 0.5f, 0.5f) make camera inside unit-cube.
        mProgram->setViewMatrix(glm::lookAt(mCamera->getCameraDirection() * glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

        glm::mat4 projectMatrix = glm::perspective(glm::radians(45.0f), (float)mWidth/(float)mHeight, 0.5f, 5000.0f);

        mProgram->setProjectionMatrix(projectMatrix);

        mProgram->setModelMatrix(glm::mat4(1.0f));

        unsigned int textureId;
        mIblTexture->getTextureId(textureId);
        mProgram->setTextureCube("skybox", textureId);

        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        glDisable(GL_DEPTH_TEST);

        glBindVertexArray(mVAO);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);        
    }

    SkyBox::~SkyBox() {

    }

}