#ifndef _MODEL_H_
#define _MODEL_H_

#include <string>
#include <iostream>
#include <vector>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "program.h"

namespace PbrGi {

    struct vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
    };

    struct texture {
        unsigned int id;
        std::string type;
        std::string path;
    };

    class mesh {
    public:
        mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices, std::vector<texture> textures);
        void drawMesh(std::shared_ptr<Program> program, int size);
        std::shared_ptr<mesh> processMesh(aiMesh* mesh, const aiScene* scene);

        ~mesh();

    private:
        void setupMesh();

    private:
        std::vector<vertex> vertices_;
        std::vector<unsigned int> indices_;
        std::vector<texture> textures_;
        unsigned int VAO_;
        unsigned int VBO_;
        unsigned int EBO_;
    };

    class model {
    public:
        model(std::string path);
        ~model();
        void drawModel(std::shared_ptr<Program> program);
        void addInstance(glm::mat4 posAndSizeMat4);

    private:
        void loadModel(std::string path);
        void processNode(aiNode* node, const aiScene* scene);

        std::shared_ptr<mesh> processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
        unsigned int textureFromFile(const char* path, const std::string& directory);

    private:
        std::vector<std::shared_ptr<mesh>> meshes_;
        std::string directory_;
        std::vector<glm::mat4> transforms_;
    };

}

#endif