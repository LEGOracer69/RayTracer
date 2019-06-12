#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>

#include <shader.hpp>
#include <color.hpp>

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
    Color color;
    // Functions
    Mesh(std::vector <Vertex> vertices, std::vector <unsigned int> indices, std::vector <Texture> textures, Color meshColor);
    void Draw(Shader& shader);
    
    Color getColor(glm::vec2 TexCoords);
    Color getColor() {return color;};
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