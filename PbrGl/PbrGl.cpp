#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "model.h"
#include "program.h"
#include "framebuffer.h"
#include "texture.h"
#include "camera.h"
#include "skybox.h"

#include <iostream>
#include "common.hpp"

#include "colorPass.h"
#include "shadowPass.h"
#include "gaussianBlurPass.h"
#include "bilateralBlurPass.h"
#include "gtaoPass.h"
#include "structurePass.h"
#include "toneMapPass.h"
#include "SsrPass.h"
#include "hierarchicalDepthPass.h"

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

    /**************opengl configure********************/
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    //glEnable(GL_POLYGON_OFFSET_FILL);
    //glPolygonOffset(1, 1);
    //glEnable(GL_DEPTH_TEST);
    GLint maxTextureUnits;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
    std::cout << "此设备支持的最大纹理单元数量为: " << maxTextureUnits << std::endl;

    /**************load model********************/
    std::vector<std::shared_ptr<PbrGi::model>> models;
    //auto su7 = std::make_shared<PbrGi::model>(".\\asset\\model\\DamagedHelmet.glb");
    //auto su7 = std::make_shared<PbrGi::model>(".\\asset\\model\\sphere.glb");
    auto su7 = std::make_shared<PbrGi::model>(".\\asset\\model\\su7.glb");
    //auto su7 = std::make_shared<PbrGi::model>(".\\asset\\model\\sponza.glb");

    std::vector<float> box = su7->get3DBox();
    glm::mat4 trans1(1.0f);
    trans1 = glm::translate(trans1, glm::vec3(0.0f, 0.0f, 0.5 * abs(box[5] - box[4])));
    trans1 = glm::rotate(trans1, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        
    trans1 = glm::translate(glm::inverse(trans1), glm::vec3(-0.5 * (box[0] + box[1]), -0.5 * (box[2] + box[3]), -0.5 * (box[4] + box[5])));
    su7->addInstance(trans1);
    models.push_back(su7);

    std::vector<float> plane = {
        -10000.0, 0.0, -10000.0,
        -10000.0, 0.0, 10000.0,
        10000.0, 0.0, 10000.0,
        10000.0, 0.0, 10000.0,
        10000.0, 0.0, -10000.0,
        -10000.0, 0.0, -10000.0,
    };
    std::shared_ptr<PbrGi::model> shadowGround = std::make_shared<PbrGi::customModel>(plane, glm::vec3(1.0, 1.0, 1.0), 3);
    shadowGround->addInstance(glm::mat4(1.0));
    models.push_back(shadowGround);

    /**************load camera********************/
    
    //kCamera = std::make_shared<PbrGi::camera>(SCR_WIDTH, SCR_HEIGHT, 25.0f, 5000.0f);
    ///std::shared_ptr<PbrGi::camera> kLight = std::make_shared<PbrGi::camera>(SCR_WIDTH, SCR_HEIGHT, 0.5f, 25000.0f, glm::vec3(0.0f, 10000.0f, -1000.0f));

    kCamera = std::make_shared<PbrGi::camera>(SCR_WIDTH, SCR_HEIGHT, 0.5f, 5000.0f, glm::vec3(-400.0, 100.0, -400.0));
    std::shared_ptr<PbrGi::camera> kLight = std::make_shared<PbrGi::camera>(SCR_WIDTH, SCR_HEIGHT, 0.5f, 5000.0f, glm::vec3(1000.0f, 1000.0f, -1000.0f));

    /**************setup renderPass********************/
    std::shared_ptr<PbrGi::SkyBox> iblSkyBox = std::make_shared<PbrGi::SkyBox>(SCR_WIDTH, SCR_HEIGHT, kCamera);

    std::shared_ptr<PbrGi::ShadowPass> kShadowPass = std::make_shared<PbrGi::ShadowPass>(kLight);

    std::shared_ptr<PbrGi::StructurePass> kStructurePass = std::make_shared<PbrGi::StructurePass>(kCamera);

    std::shared_ptr<PbrGi::GaussianBlurPass> kGaussianBlurPass = std::make_shared<PbrGi::GaussianBlurPass>(kShadowPass->result());

    std::shared_ptr<PbrGi::GtaoPass> kGtaoPass = std::make_shared<PbrGi::GtaoPass>(kCamera, kStructurePass->result());

    std::shared_ptr<PbrGi::BilateralBlurPass> kBilateralBlurPass = std::make_shared<PbrGi::BilateralBlurPass>(kGtaoPass->result(), kCamera);

    std::shared_ptr<PbrGi::ColorPass> kColorPass = std::make_shared<PbrGi::ColorPass>(kCamera, iblSkyBox, kBilateralBlurPass->result(), kGaussianBlurPass->result(), kLight);

    std::shared_ptr<PbrGi::HierarchicalDepthPass> kHierarchicalDepthPass = std::make_shared<PbrGi::HierarchicalDepthPass>(kStructurePass->result());

    std::shared_ptr<PbrGi::SSrPass> kSsrPass = std::make_shared<PbrGi::SSrPass>(kCamera, kHierarchicalDepthPass->result(), kColorPass->result(), kStructurePass->normal());

    std::shared_ptr<PbrGi::ToneMapPass> kToneMapPass = std::make_shared<PbrGi::ToneMapPass>(kColorPass->result());

    float blurWidth = 20.0f;

    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);
        
        kStructurePass->render(models);
        kShadowPass->render(models);
        kGaussianBlurPass->render(blurWidth);
        kGtaoPass->render();
        kBilateralBlurPass->render();
        kHierarchicalDepthPass->render();
        kSsrPass->render();
        kColorPass->render(models, false);
        kToneMapPass->render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources
    glfwTerminate();
    return 0;
}