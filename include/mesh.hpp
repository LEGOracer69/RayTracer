#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>

#include <shader.hpp>
#include <material.hpp>

#include <string>
#include <vector>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
}; 

class Mesh {
public:
    // Mesh Data
    std::vector <Vertex> vertices;
    std::vector <unsigned int> indices;
    std::vector <Texture> textures;
    Material material;
    // Functions
    Mesh(std::vector <Vertex> vertices, std::vector <unsigned int> indices, std::vector <Texture> textures, Material meshMaterial);
    void Draw(Shader& shader);
    
    Material getMaterial(glm::vec2 TexCoords);
    Material getMaterial() {return material;};
private:
    // Render data
    unsigned int VAO, VBO, EBO;
    // Functions
    void setupMesh();

    Texture *textureNormal = NULL;
    Texture *textureHeight = NULL;
    Texture *textureSpecular = NULL;
    Texture *textureDiffuse = NULL;
};


#endif