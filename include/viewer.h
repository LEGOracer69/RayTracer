#ifndef VIEWER_H
#define VIEWER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <camera.h>
#include <screen.h>

#include <iostream>

#define RAY_TRACER 1
#define OPEN_GL 0


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
    // Camera
    Camera camera;
    Screen screen;

    // Shaders
    Shader* viewShader;

    // Model
    Model* ourModel;

    // Variables
    GLFWwindow* window;
    int SCR_WIDTH, SCR_HEIGHT;
    int realWidth, realHeight;
    int mode;

    // Timing
    float deltaTime; // Time between current frame and last frame
    float lastFrame;

};

//Viewer::Viewer(GLFWwindow*& _window, int SCREEN_WIDTH, int SCREEN_HEIGHT) :
Viewer::Viewer(int SCREEN_WIDTH, int SCREEN_HEIGHT) :
    SCR_WIDTH(SCREEN_WIDTH), SCR_HEIGHT(SCREEN_HEIGHT),
    camera(window, SCR_WIDTH, SCR_HEIGHT, realWidth, realHeight, deltaTime, lastFrame),
    ourModel(NULL), deltaTime(0.0f), lastFrame(0.0f), mode(0)
{
}

void Viewer::compileShaders(){
    viewShader = new Shader("shader/simple_vs.glsl", "shader/simple_fs.glsl");
    screen.compileShader();

    screen.init();
}

bool Viewer::createWindow(){ // Change OpenGL Settings
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OBJ Loader", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwSetWindowUserPointer(window, &camera);

    glfwMakeContextCurrent(window);


    camera.setCallbacks(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    glClearColor(0.5f, 0.3f, 0.3f, 1.0f);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // #################################
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    // #################################

    return true;
}

void Viewer::processInput() {
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    camera.processInput(window);
}

void Viewer::setImage(uint8_t* data) {
    screen.createImage(data, SCR_WIDTH, SCR_HEIGHT);
}

void Viewer::setMode(int _mode){
    mode = _mode;
    if(_mode == OPEN_GL){
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
    }
}

bool Viewer::update(){
    if(!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput();

        // render
        // ------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(mode == OPEN_GL) {
            glm::mat4 model, view, projection;
            view = camera.GetViewMatrix();
            projection = glm::perspective(glm::radians(camera.Zoom),
                (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            
            viewShader->use();
            viewShader->setMat4("model", model);
            viewShader->setMat4("view", view);
            viewShader->setMat4("projection", projection);
            if(ourModel) {
                ourModel->Draw(*viewShader);
            }
        } else if(mode == RAY_TRACER) {
            screen.draw();
        }


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        return true;
    }

    // -- else --

    // glDeleteVertexArrays(vertices.size(), VAO);
    // glDeleteBuffers(vertices.size(), VBO);
    // glDeleteBuffers(10, EBO);
    glfwTerminate();
    return false;
}










// Callbacks

/*
void Viewer::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Viewer::mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) // this bool variable is initially set to true
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    //camera.ProcessMouseMovement(xoffset, yoffset);
    ProcessMouseMovement(xoffset, yoffset);
}

void Viewer::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    ProcessMouseScroll(yoffset);
}
*/


#endif