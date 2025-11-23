#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "model.h"
#include "program.h"
#include "framebuffer.h"
#include "texture.h"
#include "camera.h"
#include "skybox.h"

#include <iostream>

#define SCR_WIDTH 2000
#define SCR_HEIGHT 1200

float kReleaseMouseX = 0.0f, kReleaseMouseY = 0.0f;
float kPushMouseX = 0.0f, kPushMouseY = 0.0f;
bool kIfMouseRelease = true;
double mousePosX, mousePosY;

std::shared_ptr<PbrGi::camera> kCamera;

void processInput(GLFWwindow* window)
{
    float cameraMoveSpeed = 1.0f;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        kCamera->move(PbrGi::MOVE_FRONT, cameraMoveSpeed);
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        kCamera->move(PbrGi::MOVE_BACK, cameraMoveSpeed);
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        kCamera->move(PbrGi::MOVE_LEFT, cameraMoveSpeed);
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        kCamera->move(PbrGi::MOVE_RIGHT, cameraMoveSpeed);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        float xoffset = xpos - mousePosX;
        float yoffset = ypos - mousePosY;
        mousePosX = xpos;
        mousePosY = ypos;
        float sensitivity = 0.1;
        xoffset *= sensitivity;
        yoffset *= sensitivity;
        kCamera->viewAngle(xoffset, yoffset);
    }
    else {
        mousePosX = xpos;
        mousePosY = ypos;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    kCamera->zoom(yoffset);
}

int main() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    //glfwWindowHint(GLFW_SAMPLES, 4);

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "PbrGi", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    //std::shared_ptr<PbrGi::Program> simpleShader = std::make_shared<PbrGi::Program>("C:\\Users\\zhuhui\\source\\repos\\PbrGl\\PbrGl\\shader\\SimpleVertex.glsl", "C:\\Users\\zhuhui\\source\\repos\\PbrGl\\PbrGl\\shader\\SimpleFragment.glsl");

    std::vector<std::string> textureNames = {"baseColorTexture", "sampler0_iblDFG", "sampler0_iblSpecular", "roughnessTexture", "normalTexture", "metalnessTexture"};
    std::shared_ptr<PbrGi::Program> simpleShader = std::make_shared<PbrGi::Program>(textureNames, ".\\shader\\pbrVertex.glsl", ".\\shader\\pbrFragment.glsl");

    //auto su7 = std::make_shared<PbrGi::model>("C:\\Users\\zhuhui\\Desktop\\pile.glb");
    //auto su7 = std::make_shared<PbrGi::model>(".\\asset\\model\\sphere.glb");
    auto su7 = std::make_shared<PbrGi::model>(".\\asset\\model\\su7.glb");
    std::vector<float> box = su7->get3DBox();
    glm::mat4 trans1(1.0f);
    trans1 = glm::rotate(trans1, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    
    trans1 = glm::translate(glm::inverse(trans1), glm::vec3(-0.5 * (box[0] + box[1]), -0.5 * (box[2] + box[3]), -0.5 * (box[4] + box[5])));
    su7->addInstance(trans1);

    kCamera = std::make_shared<PbrGi::camera>(SCR_WIDTH, SCR_HEIGHT, 0.5f, 5000.0f, glm::vec3(-400.0, 100.0, -400.0));
    //kCamera = std::make_shared<PbrGi::camera>(SCR_WIDTH, SCR_HEIGHT, 0.5f, 5000.0f);


    std::shared_ptr<PbrGi::frameBuffer> indirectLightFramebuffer = std::make_shared<PbrGi::frameBuffer>(SCR_WIDTH, SCR_HEIGHT, false, true, 8);
    indirectLightFramebuffer->init();

    std::shared_ptr<PbrGi::Texture> ibl = std::make_shared<PbrGi::Texture>();
    std::vector<std::string> prefilterIbl = {
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
    unsigned int prefilterIblMimmap = 5;
    ibl->initCubeTextureHDR(prefilterIbl, prefilterIblMimmap);

    std::shared_ptr<PbrGi::SkyBox> iblSkyBox = std::make_shared<PbrGi::SkyBox>(SCR_WIDTH, SCR_HEIGHT, kCamera, ibl);

    std::shared_ptr<PbrGi::Texture> dfg = std::make_shared<PbrGi::Texture>();
    dfg->init2DTextureHDR(".\\asset\\dfg\\dfg.hdr", false);

    //glEnable(GL_POLYGON_OFFSET_FILL);
    //glPolygonOffset(1, 1);
    //glEnable(GL_DEPTH_TEST);

    float sh[] = {
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

    simpleShader->use();
    unsigned int id;
    ibl->getTextureId(id);
    simpleShader->setTextureCube("sampler0_iblSpecular", id);

    unsigned int id1;
    dfg->getTextureId(id1);
    simpleShader->setTexture2D("sampler0_iblDFG", id1);

    simpleShader->setFloat("sampler0_iblSpecular_mipmapLevel", 4);

    simpleShader->setVec3Float("iblSH", sh, 9);

    GLint maxTextureUnits;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
    std::cout << "此设备支持的最大纹理单元数量为: " << maxTextureUnits << std::endl;


    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        indirectLightFramebuffer->setup();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        iblSkyBox->render();

        simpleShader->use();
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        simpleShader->setViewMatrix(kCamera->getViewMatrix());
        simpleShader->setProjectionMatrix(kCamera->getProjectMatrix());
        simpleShader->setProperty(kCamera->getCameraPosition(), "cameraPosition");

        su7->drawModel(simpleShader);

        indirectLightFramebuffer->unload();
        indirectLightFramebuffer->blitToFrameBuffer(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources
    glfwTerminate();
    return 0;
}