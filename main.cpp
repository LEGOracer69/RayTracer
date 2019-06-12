#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>

#include <shader.hpp>
#include <camera.hpp>
#include <model.hpp>
#include <renderer.hpp>
#include <scene.hpp>
#include <viewer.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

void processInput(GLFWwindow *window);

// display mode
bool TRACING = false;
bool startRender = false;


int main() {
    Scene scene("data/view_test.rtc");
    Viewer viewer(scene.scr_width, scene.scr_height);
    viewer.createWindow();
    
    GLFWwindow* window = viewer.getWindow();

    Camera& camera = *viewer.getCamera();


    Model ourModel("data/" + scene.filename_in);
    // Model ourModel("data/nanosuit.obj");
    // Model ourModel("data/teapot.obj");
    // Model ourModel;
    viewer.setModel(&ourModel);
    viewer.compileShaders();


    Renderer renderer(ourModel.meshes, scene);
    // Renderer renderer(ourModel.meshes, scene);
    // Scene scene("a");
    uint8_t* data = renderer.getImage();
    viewer.setImage(data);

    while(viewer.update()) {
        processInput(window);
        if(TRACING)
            viewer.setMode(RAY_TRACER);
        else
            viewer.setMode(OPEN_GL);
        
        if(startRender) {
            startRender = false;
            renderer.render(camera.Position, camera.Front, 45.0f);
            viewer.setImage(data);
            renderer.save();
        }
    }
}


void processInput(GLFWwindow *window) {
    static bool C_PRESSED = false;
    static bool R_PRESSED = false;

    if ((glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)){
        if(!C_PRESSED) {
            TRACING = !TRACING;
        }
        C_PRESSED = true;
    } else
        C_PRESSED = false;

    if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)){
        if(!R_PRESSED) {
            startRender = true;
        }
        R_PRESSED = true;
    } else
        R_PRESSED = false;

    // if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)){
    //     startRender = true;
    // }
}
