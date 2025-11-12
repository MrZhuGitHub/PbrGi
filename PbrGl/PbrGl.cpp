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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
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

    std::shared_ptr<PbrGi::Program> simpleShader = std::make_shared<PbrGi::Program>("C:\\Users\\zhuhui\\source\\repos\\PbrGl\\PbrGl\\shader\\SimpleVertex.glsl", "C:\\Users\\zhuhui\\source\\repos\\PbrGl\\PbrGl\\shader\\SimpleFragment.glsl");

    auto su7 = std::make_shared<PbrGi::model>("C:\\Users\\zhuhui\\source\\repos\\PbrGl\\PbrGl\\asset\\su7.glb");

    std::vector<float> box = su7->get3DBox();
    glm::mat4 trans1(1.0f);
    trans1 = glm::rotate(trans1, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    
    std::cout << 0.5 * (box[0] + box[1]) << "," << 0.5 * (box[2] + box[3]) << "," << 0.5 * (box[4] + box[5]) << std::endl;
    std::cout << abs(box[1] - box[0]) << "," << abs(box[3] - box[2]) << "," << abs(box[5] - box[4]) << std::endl;
    trans1 = glm::translate(glm::inverse(trans1), glm::vec3(-0.5 * (box[0] + box[1]), -0.5 * (box[2] + box[3]), -0.5 * (box[4] + box[5])));
    su7->addInstance(trans1);

    kCamera = std::make_shared<PbrGi::camera>(SCR_WIDTH, SCR_HEIGHT, 0.5f, 5000.0f);

    std::shared_ptr<PbrGi::frameBuffer> indirectLightFramebuffer = std::make_shared<PbrGi::frameBuffer>(SCR_WIDTH, SCR_HEIGHT, false, true, 8);
    indirectLightFramebuffer->init();

    std::shared_ptr<PbrGi::Texture> ibl = std::make_shared<PbrGi::Texture>();
    std::vector<std::string> prefilterIbl = {
            "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m0_px.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m0_nx.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m0_py.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m0_ny.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m0_pz.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m0_nz.png",
                "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m1_px.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m1_nx.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m1_py.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m1_ny.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m1_pz.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m1_nz.png",
                "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m2_px.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m2_nx.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m2_py.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m2_ny.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m2_pz.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m2_nz.png",
                "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m3_px.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m3_nx.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m3_py.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m3_ny.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m3_pz.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m3_nz.png",
                "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m4_px.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m4_nx.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m4_py.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m4_ny.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m4_pz.png",
        "C:\\Users\\zhuhui\\Desktop\\pbr\\test\\qwantani_noon_puresky_4k\\m4_nz.png",
    };
    unsigned int prefilterIblMimmap = 5;
    if (ibl->initCubeTexture(prefilterIbl, prefilterIblMimmap)) {
        std::cout << "init ibl success" << std::endl;
    } else {
        std::cout << "init ibl failed" << std::endl;
    }

    std::shared_ptr<PbrGi::SkyBox> iblSkyBox = std::make_shared<PbrGi::SkyBox>(SCR_WIDTH, SCR_HEIGHT, kCamera, ibl);

    //glEnable(GL_POLYGON_OFFSET_FILL);
    //glPolygonOffset(1, 1);
    //glEnable(GL_DEPTH_TEST);

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

        unsigned int id;
        ibl->getTextureId(id);
        simpleShader->setTextureCube("ibl", id);

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