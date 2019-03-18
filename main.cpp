#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h> //this new

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>

#include <shader.h>
#include <camera.h>
#include <model.h>
#include <renderer.h>
#include <scene.h>
#include <light.h>
#include <viewer.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

void processInput(GLFWwindow *window);

// display mode
bool TRACING = false;
bool startRender = false;


int main(int argc, char *argv[]) {
    Scene scene("data/view_test.rtc");
    Viewer viewer(scene.scr_width, scene.scr_height);
    viewer.createWindow();
    
    GLFWwindow* window = viewer.getWindow();

    Camera& camera = *viewer.getCamera();


    Model ourModel("data/" + scene.filename_in);
    viewer.setModel(&ourModel);
    viewer.compileShaders();



    Renderer renderer(ourModel.meshes, scene.scr_width, scene.scr_height, scene.rec_depth);
    renderer.render(camera.Position, {0.0f, 0.0f, -1.0f}, 45.0f);
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
        startRender = true;
    }
}
