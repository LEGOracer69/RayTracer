#ifndef VIEWER_H
#define VIEWER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <camera.hpp>
#include <screen.hpp>

#include <iostream>

#define RAY_TRACER 1
#define OPEN_GL 0

class Model;

class Viewer {
public:
    // Constructors
    //Viewer(GLFWwindow*& _window, int SCREEN_WIDTH, int SCREEN_HEIGHT);
    Viewer(int SCREEN_WIDTH, int SCREEN_HEIGHT);
    ~Viewer(){delete viewShader;};

    // Functions
    bool createWindow();
    GLFWwindow* getWindow() {return window;};
    Camera* getCamera() {return &camera;};
    void processInput();
    void setScene();
    void setModel(Model* model) {ourModel = model;};
    void compileShaders();
    void setMode(int _mode);
    void setImage(uint8_t* data);

    // Loop
    bool update();

private:
    // Variables
    GLFWwindow* window;
    int SCR_WIDTH, SCR_HEIGHT;
    int realWidth, realHeight;
    int mode;

    // Camera
    Camera camera;
    Screen screen;

    // Shaders
    Shader* viewShader;

    // Model
    Model* ourModel;

    // Timing
    float deltaTime; // Time between current frame and last frame
    float lastFrame;
};

#endif