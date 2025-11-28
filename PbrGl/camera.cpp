#include "camera.h"

#include <iostream>
#include <cmath>

namespace PbrGi {

    camera::camera(float width, float height, float near, float far, glm::vec3 cameraPos, glm::vec3 cameraFocus, glm::vec3 cameraUp)
        : width_(width)
        , height_(height)
        , near_(near)
        , far_(far)
        , cameraPos_(cameraPos)
        , cameraFocus_(cameraFocus)
        , cameraUp_(cameraUp)
        , cameraDirection_(glm::normalize(cameraPos_ - cameraFocus_))
        , fov_(glm::length(cameraPos - cameraFocus))
        , yaw_(90.0f)
        , pitch_(0.0f) {

        yaw_ = atan(cameraDirection_.z / cameraDirection_.x) * 180.0f / 3.14159 + 180.0f;
        pitch_ = atan(cameraDirection_.y / sqrt(cameraDirection_.z * cameraDirection_.z + cameraDirection_.x * cameraDirection_.x)) * 180.0f / 3.14159;
    }

    void camera::move(MoveDirection direction, float distance) {

        glm::vec3 x(cameraDirection_.x, 0, cameraDirection_.z);
        x = glm::normalize(x);
        glm::vec3 y = glm::cross(x, glm::vec3(0, 1, 0));
        y = glm::normalize(y);
        if (MOVE_FRONT == direction) {
            cameraFocus_ = cameraFocus_ - x * distance;
        }
        else if (MOVE_BACK == direction) {
            cameraFocus_ = cameraFocus_ + x * distance;
        }
        else if (MOVE_LEFT == direction) {
            cameraFocus_ = cameraFocus_ + y * distance;
        }
        else if (MOVE_RIGHT == direction) {
            cameraFocus_ = cameraFocus_ - y * distance;
        }
        else {
            std::cout << "input invalid camera moving direction" << std::endl;
        }

        cameraPos_ = fov_ * cameraDirection_ + cameraFocus_;
    }

    void camera::viewAngle(float yawOffset, float pitchOffset) {

        pitch_ = pitch_ + pitchOffset;
        yaw_ = yaw_ + yawOffset;
        if (pitch_ > 89.0f)
            pitch_ = 89.0f;
        if (pitch_ < -89.0f)
            pitch_ = -89.0f;

        cameraDirection_.x = cos(glm::radians(pitch_)) * cos(glm::radians(yaw_));
        cameraDirection_.y = sin(glm::radians(pitch_));
        cameraDirection_.z = cos(glm::radians(pitch_)) * sin(glm::radians(yaw_));
        cameraPos_ = fov_ * cameraDirection_ + cameraFocus_;
    }

    void camera::zoom(float fovOffset) {
        fov_ = fov_ + fovOffset * 10.0;
        if (fov_ < 1) {
            fov_ = 1;
        }
        cameraPos_ = fov_ * cameraDirection_ + cameraFocus_;
    }

    glm::mat4 camera::getViewMatrix() {
        glm::mat4 view;
        view = glm::lookAt(cameraPos_, cameraFocus_, cameraUp_);
        return view;
    }

    glm::mat4 camera::getProjectMatrix() {
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), width_ / height_, near_, far_);
        return proj;
    }

    glm::vec3 camera::getCameraPosition() {
        return cameraPos_;
    }

}