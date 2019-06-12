#ifndef SCREEN_H
#define SCREEN_H

#include <glad/glad.h>
#include <shader.hpp>

class Screen{
public:
    Screen(){};
    ~Screen(){delete screenShader;};
    void init();
    void compileShader(){screenShader = new Shader("shader/screen_vs.glsl", "shader/screen_fs.glsl");};
    void createImage(uint8_t* data, int width, int height);
    void draw();
private:
    float vertices[32] = {
        // positions          // colors           // texture coords
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         1.0f,  1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -1.0f, -1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[6] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO, texture1;

    Shader* screenShader;
};

#endif