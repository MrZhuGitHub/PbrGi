#include "program.h"

#include "glm/gtc/type_ptr.hpp"

namespace PbrGi {

    Program::Program(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;

        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            if (nullptr != geometryPath) {
                gShaderFile.open(geometryPath);
            }
            std::stringstream vShaderStream, fShaderStream, gShaderStream;

            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            if (nullptr != geometryPath) {
                gShaderStream << gShaderFile.rdbuf();
            }

            vShaderFile.close();
            fShaderFile.close();
            if (nullptr != geometryPath) {
                gShaderFile.close();
            }

            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            if (nullptr != geometryPath) {
                geometryCode = gShaderStream.str();
            }
        }
        catch (std::ifstream::failure e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        const char* gShaderCode = geometryCode.c_str();

        unsigned int vertex, fragment, geometry;
        int success;
        char infoLog[512];

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        };

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        if (nullptr != geometryPath) {
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(geometry, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
            }
        }

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if (nullptr != geometryPath) {
            glAttachShader(ID, geometry);
        }
        glLinkProgram(ID);

        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteShader(geometry);
    }

    void Program::use() {
        glUseProgram(ID);
    }

    void Program::setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void Program::setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void Program::setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void Program::setViewMatrix(glm::mat4 viewMatrix) {
        glUniformMatrix4fv(glGetUniformLocation(ID, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    }

    void Program::setProjectionMatrix(glm::mat4 projectionMatrix) {
        glUniformMatrix4fv(glGetUniformLocation(ID, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    }

    void Program::setModelMatrix(glm::mat4 modelMatrix) {
        glUniformMatrix4fv(glGetUniformLocation(ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    }

    void Program::setProperty(float* property, std::string name) {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, property);
    }

    void Program::setFloat(float property, std::string name) {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), property);
    }

    void Program::setObjPosMatrix(glm::mat4 objMatrix) {
        glUniformMatrix4fv(glGetUniformLocation(ID, "objPosMatrix"), 1, GL_FALSE, glm::value_ptr(objMatrix));
    }

    void Program::setProperty(glm::mat4 property, std::string name) {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(property));
    }

    void Program::setProperty(glm::vec3 property, std::string name) {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(property));
    }

    void Program::setProperty(glm::vec2 property, std::string name) {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(property));
    }

    void Program::setProperty(glm::ivec2 property, std::string name) {
        //std::cout << "1:" << glGetError() << std::endl;
        glUniform2i(glGetUniformLocation(ID, name.c_str()), property[0], property[1]);
        //std::cout << "2:" << glGetError() << std::endl;
    }

    Program::~Program() {
        glDeleteProgram(ID);
    }

}