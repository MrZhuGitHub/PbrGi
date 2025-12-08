#ifndef _MODEL_H_
#define _MODEL_H_

#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <optional>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "program.h"

namespace PbrGi {

    class Texture;

    struct vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
        glm::vec3 tangent;
    };

    struct material {
        std::optional<glm::vec3> baseColor;
        std::optional<float> metallic;
        std::optional<float> roughness;
        std::optional<float> opacityFactor;
        std::optional<float> clearCoat;
        std::optional<float> clearCoatRoughness;
        std::optional < std::shared_ptr<PbrGi::Texture>> baseColorTexture;
        std::optional < std::shared_ptr<PbrGi::Texture>> roughnessTexture;
        std::optional < std::shared_ptr<PbrGi::Texture>> normalTexture;
        std::optional < std::shared_ptr<PbrGi::Texture>> metalnessTexture;
        std::optional < std::shared_ptr<PbrGi::Texture>> emissiveTexture;
    };

    class mesh {
    public:
        mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices, material ma);
        void drawMesh(std::shared_ptr<Program> program, int size);
        std::shared_ptr<mesh> processMesh(aiMesh* mesh, const aiScene* scene);

        ~mesh();

    private:
        void setupMesh();

    private:
        std::vector<vertex> vertices_;
        std::vector<unsigned int> indices_;
        unsigned int VAO_;
        unsigned int VBO_;
        unsigned int EBO_;

    public:
        material material_;
    };

    class model {
    public:
        model(std::string path);
        ~model();
        virtual void drawModel(std::shared_ptr<Program> program);
        virtual void addInstance(glm::mat4 posAndSizeMat4);
        std::vector<float> get3DBox() {
            std::vector<float> box = { mXmin , mXmax ,mYmin ,mYmax ,mZmin ,mZmax };
            return box;
        }

    private:
        void loadModel(std::string path);
        void processNode(aiNode* node, const aiScene* scene);

        std::shared_ptr<mesh> processMesh(aiMesh* mesh, const aiScene* scene);

    private:
        std::vector<std::shared_ptr<mesh>> meshes_;
        std::string directory_;

    protected:
        float mXmax;
        float mYmax;
        float mZmax;
        float mXmin;
        float mYmin;
        float mZmin;

        std::vector<glm::mat4> transforms_;
    };

    class customModel : public model {
        public:
            //vec3 vPosition; vec2 aTexCoord; vec3 vNormal;
            customModel(std::vector<float> geometryData, glm::vec3 color, unsigned int stride);
            virtual void drawModel(std::shared_ptr<Program> program) override;
            ~customModel();

        private:
            std::vector<vertex> vertices_;
            unsigned int mVAO;
            unsigned int mVBO;
            glm::vec3 mColor;
                };

}

#endif