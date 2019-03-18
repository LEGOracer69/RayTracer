#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

    // Eular Angles
    float Yaw;
    float Pitch;

    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

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


// Constructor
Camera::Camera(GLFWwindow* window, int& SCREEN_WIDTH, int& SCREEN_HEIGHT,
                                   int& _realWidth, int& _realHeight,
                                   float& _deltaTime, float& _lastFrame) :
    Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
    Position(glm::vec3(0.0f, 0.0f, 2.0f)),
    WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
    MouseSensitivity(0.1f),
    MovementSpeed(2.5f),
    Zoom(45.0f),
    Yaw(-90.0f),
    Pitch(0.0f),
    SCR_WIDTH(SCREEN_WIDTH), SCR_HEIGHT(SCREEN_HEIGHT),
    realWidth(_realWidth), realHeight(_realHeight),
    deltaTime(_deltaTime), lastFrame(_lastFrame)
{
    updateCameraVectors();
}

// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
    if (direction == UPWARD)
        Position -= Up * velocity;
    if (direction == DOWNWARD)
        Position += Up * velocity;
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f) 
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // Update Front, Right and Up Vectors using the updated Eular angles
    updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset)
{
    std::cout << Zoom << "\n";

    if (Zoom >= 1.0f && Zoom <= 45.0f)
        Zoom -= yoffset;
    if (Zoom <= 1.0f)
        Zoom = 1.0f;
    if (Zoom >= 45.0f)
        Zoom = 45.0f;
}
// Calculates the front vector from the Camera's (updated) Eular Angles
void Camera::updateCameraVectors()
{
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp)); // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        ProcessKeyboard(UPWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        ProcessKeyboard(DOWNWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        MovementSpeed = 12.5f;
    else
        MovementSpeed = 2.5f;
}

void Camera::setCallbacks(GLFWwindow* window) {
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height){ 
        static_cast<Camera*>(glfwGetWindowUserPointer(window))->framebuffer_size_callback(window, width, height);
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos){ 
        static_cast<Camera*>(glfwGetWindowUserPointer(window))->mouse_callback(window, xpos, ypos);
    });
    glfwSetScrollCallback(window, [](GLFWwindow *window, double xoffset, double yoffset){
        static_cast<Camera*>(glfwGetWindowUserPointer(window))->scroll_callback(window, xoffset, yoffset);
    });
}

void Camera::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Camera::mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    static bool firstMouse = true;
    static float lastX = SCR_WIDTH / 2;
    static float lastY = SCR_HEIGHT / 2;
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

    ProcessMouseMovement(xoffset, yoffset);
}

void Camera::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    ProcessMouseScroll(yoffset);
}

/*
int intify(std::string s) {
    int result = 0;
    for (int i = int(s[0] == '-'); i < s.size(); i++)
        result += (s[i] - '0') * int(std::round(glm::pow(10, s.size() - i - 1)));
    if (s[0] == '-')
        result *= -1;
    return result;
}
*/

#endif