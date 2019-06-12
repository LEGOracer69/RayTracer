#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UPWARD,
    DOWNWARD
};

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Constructors
    Camera(GLFWwindow* window, int& SCREEN_WIDTH, int& SCREEN_HEIGHT,
        int& _realWidth, int& _realHeight, float& _deltaTime, float& _lastFrame);

    //Functions
    glm::mat4 GetViewMatrix();
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch);
    void ProcessMouseScroll(float yoffset);
    void updateCameraVectors();
    void processInput(GLFWwindow* window);
    void setCallbacks(GLFWwindow* window);

    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Camera options
    float MouseSensitivity;
    float MovementSpeed;
    float Zoom;

    // Eular Angles
    float Yaw;
    float Pitch;

private:
    // Callbacks
    void mouse_callback(GLFWwindow *window, double xpos, double ypos);
    void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

    // screen
    int& SCR_WIDTH;
    int& SCR_HEIGHT;
    int& realWidth;
    int& realHeight;

    // timing
    float& deltaTime; // Time between current frame and last frame
    float& lastFrame; // Time of last frame
};
#endif