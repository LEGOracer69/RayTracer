#ifndef MODEL_H
#define MODEL_H

#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <mesh.hpp>
#include <scene.hpp>

#include <string>
#include <vector>


unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

class Model {
public:
    // Functions
    Model(Scene& scene) { loadModel(scene); };
    void Draw(Shader& shader);
    // Model data
    std::vector <Mesh> meshes;
private:
    std::string directory;
    std::vector<Texture> textures_loaded; // stores all the textures loaded so far
    // Functions
    void loadModel(Scene& scene);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector <Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};



#endif