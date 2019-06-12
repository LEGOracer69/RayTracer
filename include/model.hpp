#ifndef MODEL_H
#define MODEL_H

#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <mesh.hpp>
#include <color.hpp>

#include <string>
#include <vector>


unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

class Model {
public:
    // Functions
    Model(std::string path) { loadModel(path); };
    void Draw(Shader& shader);
    bool intersectModel(const glm::vec3& origin, const glm::vec3& ray, glm::vec3& touch, glm::vec3& normal, Color& color);
    // Model data
    std::vector <Mesh> meshes;
private:
    std::string directory;
    std::vector<Texture> textures_loaded; // stores all the textures loaded so far
    // Functions
    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector <Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};



#endif