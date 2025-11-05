#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>

namespace PbrGi {

    class Program {
    public:
        Program(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
        ~Program();

        void use();

        void setBool(const std::string& name, bool value) const;
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;

        void setViewMatrix(glm::mat4 viewMatrix);
        void setProjectionMatrix(glm::mat4 projectionMatrix);
        void setModelMatrix(glm::mat4 modelMatrix);
        void setObjPosMatrix(glm::mat4 objMatrix);

        void setProperty(float* property, std::string name);

        void setProperty(glm::mat4 property, std::string name);

        void setProperty(glm::vec3 property, std::string name);

        void setProperty(glm::vec2 property, std::string name);

        void setFloat(float property, std::string name);

        void setProperty(glm::ivec2 property, std::string name);

        void setLight();

    private:
        unsigned int ID;
    };

}

#endif