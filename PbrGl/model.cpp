#include "model.h"
#include "texture.h"

#include "stb_image.h"

#include <algorithm>

namespace PbrGi {

    mesh::mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices, material ma)
        : vertices_(vertices)
        , indices_(indices)
        , material_(ma) {
        setupMesh();
    }

    mesh::~mesh() {
        glDeleteVertexArrays(1, &VAO_);
        glDeleteBuffers(1, &VBO_);
        glDeleteBuffers(1, &EBO_);
    }

    void mesh::setupMesh() {

        glGenVertexArrays(1, &VAO_);
        glGenBuffers(1, &VBO_);
        glGenBuffers(1, &EBO_);

        glBindVertexArray(VAO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);

        glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(vertex), &vertices_[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);

        // vertex position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
        glEnableVertexAttribArray(0);
        // vertex normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
        glEnableVertexAttribArray(1);
        // vectex texCoords 
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texCoords));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

    }

    void mesh::drawMesh(std::shared_ptr<Program> program, int size) {
        program->use();

        if (material_.opacityFactor.value() == 1.0) {
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
        }
        else {
            glDepthMask(GL_FALSE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }

        if (material_.baseColor.has_value()) {
            program->setProperty(material_.baseColor.value(), "baseColor");
        }

        if (material_.opacityFactor.has_value()) {
            program->setProperty(material_.opacityFactor.value(), "opacityFactor");
        }

        if (material_.baseColorTexture.has_value()) {
            program->setBool("baseColorTextureExist", true);
            unsigned int id;
            if (material_.baseColorTexture.value()->getTextureId(id)) {
                program->setTexture2D("baseColorTexture", id);
            }
            
        }
        else {
            program->setBool("baseColorTextureExist", false);
        }

        glBindVertexArray(VAO_);

        glDrawElementsInstanced(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0, size);

        glBindVertexArray(0);

        if (material_.opacityFactor.value() != 1.0) {
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
        }
    }

    model::model(std::string path)
        : mXmax(-100000.0f)
        , mYmax(-100000.0f)
        , mZmax(-100000.0f)
        , mXmin(100000.0f)
        , mYmin(100000.0f)
        , mZmin(100000.0f) {
        std::cout << path << std::endl;
        loadModel(path);
    }

    model::~model() {

    }

    void model::drawModel(std::shared_ptr<Program> program) {
        for (int i = 0; i < transforms_.size(); i++) {
            std::string name("objPosMatrix[");
            name.append(std::to_string(i));
            name.append("]");
            program->setProperty(transforms_[i], name.c_str());
        }
        for (auto& it : meshes_) {
            it->drawMesh(program, transforms_.size());
        }
    }

    void model::addInstance(glm::mat4 posAndSizeMat4) {
        transforms_.push_back(posAndSizeMat4);
    }

    void model::loadModel(std::string path) {
        Assimp::Importer import;
        const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
            return;
        }

        directory_ = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);

        std::sort(meshes_.begin(), meshes_.end(), [](const std::shared_ptr<mesh>& a, const std::shared_ptr<mesh>& b) { return a->material_.opacityFactor.value() > b->material_.opacityFactor.value(); });
    }


    void model::processNode(aiNode* node, const aiScene* scene) {

        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes_.push_back(processMesh(mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    std::shared_ptr<mesh> model::processMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<vertex> vertices;
        std::vector<unsigned int> indices;
        material ma;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            vertex v;

            v.position.x = mesh->mVertices[i].x;
            v.position.y = mesh->mVertices[i].y;
            v.position.z = mesh->mVertices[i].z;

            mXmax = v.position.x > mXmax ? v.position.x : mXmax;
            mYmax = v.position.y > mYmax ? v.position.y : mYmax;
            mZmax = v.position.z > mZmax ? v.position.z : mZmax;
            mXmin = v.position.x < mXmin ? v.position.x : mXmin;
            mYmin = v.position.y < mYmin ? v.position.y : mYmin;
            mZmin = v.position.z < mZmin ? v.position.z : mZmin;

            v.normal.x = mesh->mNormals[i].x;
            v.normal.y = mesh->mNormals[i].y;
            v.normal.z = mesh->mNormals[i].z;

            if (mesh->mTextureCoords[0]) {
                v.texCoords.x = mesh->mTextureCoords[0][i].x;
                v.texCoords.y = mesh->mTextureCoords[0][i].y;
            }
            else {
                v.texCoords.x = 0;
                v.texCoords.y = 0;
            }

            vertices.push_back(v);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            aiColor3D baseColor;
            if (material->Get(AI_MATKEY_BASE_COLOR, baseColor) == AI_SUCCESS) {
                //std::cout << "baseColor:" << baseColor.r << "," << baseColor.g << "," << baseColor.b << std::endl;
                ma.baseColor = glm::vec3(baseColor.r, baseColor.g, baseColor.b);
            }

            float metallic = 0.0f;
            if (material->Get(AI_MATKEY_METALLIC_FACTOR, metallic) == AI_SUCCESS) {
                //std::cout << "metallic:" << metallic << std::endl;
                ma.metallic = metallic;
            }

            float roughness = 0.0f;
            if (material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == AI_SUCCESS) {
                //std::cout << "roughness:" << roughness << std::endl;
                ma.roughness = roughness;
            }

            float opacityFactor = 1.0f;
            if (material->Get(AI_MATKEY_OPACITY, opacityFactor) == AI_SUCCESS) {
                //std::cout << "opacityFactor: " << opacityFactor << std::endl;
                ma.opacityFactor = opacityFactor;
            }
            else {
                ma.opacityFactor = 1.0;
            }

            unsigned int count = material->GetTextureCount(aiTextureType_BASE_COLOR);
            if (count > 0) {
                aiString aiPath; // 存储纹理路径或嵌入式纹理标识
                if (material->GetTexture(aiTextureType_BASE_COLOR, 0, &aiPath) == AI_SUCCESS) {
                    if (aiPath.data[0] == '*') {
                        int textureIndex = std::stoi(aiPath.C_Str() + 1);
                        if (textureIndex >= 0 && textureIndex < (int)scene->mNumTextures) {
                            aiTexture* embeddedTex = scene->mTextures[textureIndex];
                            if (embeddedTex->mHeight == 0) {
                                int width, height, nrComponents;
                                auto baseColorTexture = std::make_shared<Texture>();
                                
                                if (baseColorTexture->init2DTexture(reinterpret_cast<const unsigned char*>(embeddedTex->pcData), embeddedTex->mWidth, true)) {
                                    unsigned int textureId;
                                    baseColorTexture->getTextureId(textureId);
                                    ma.baseColorTexture = baseColorTexture;
                                }                              
                            } else {
                                std::cout << "no yasuo" << std::endl;
                            }
                        }
                    } else {
                        std::cout << "no qian ru shi" << std::endl;
                    }
                }
            }
        }

        return std::make_shared<PbrGi::mesh>(vertices, indices, ma);
    }
}


