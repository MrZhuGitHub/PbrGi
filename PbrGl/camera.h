#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace PbrGi {

    enum MoveDirection {
        MOVE_FRONT,
        MOVE_BACK,
        MOVE_LEFT,
        MOVE_RIGHT,
    };

    class camera {
    public:
        camera(float width, float height, float near, float far, glm::vec3 cameraPos = glm::vec3(-20.0, 20.0, -20.0), glm::vec3 cameraFocus = glm::vec3(0, 0, 0), glm::vec3 cameraUp = glm::vec3(0, 1, 0));

        void move(MoveDirection direction, float distance);

        void viewAngle(float yawOffset, float pitchOffset);

        void zoom(float fovOffset);

        glm::mat4 getViewMatrix();

        glm::mat4 getProjectMatrix();

        glm::vec3 getCameraPosition();

        glm::vec3 getCameraDirection();

        float near();

        float far();

    private:
        glm::vec3 cameraPos_;
        glm::vec3 cameraFocus_;
        glm::vec3 cameraUp_;
        glm::vec3 cameraDirection_;
        float fov_;
        float yaw_;
        float pitch_;

        float width_;
        float height_;
        float near_;
        float far_;
    };

}

#endif