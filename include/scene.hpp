#ifndef SCENE_H
#define SCENE_H

#include <glm/glm.hpp>

#include <light.hpp>

#include <string>
#include <vector>

class Scene{
public:
    Scene(std::string filename);
    // methods

    // data
    std::string filename_in;
    std::string filename_out;

    int rec_depth;
    int scr_width;
    int scr_height;
    int samples = 1000;

    glm::vec3 VP;
    glm::vec3 LA;
    glm::vec3 UP;
    
    float yview;
    std::vector <LightPoint> lightPoint;
    std::vector <LightTriangle> lightTriangle;
};

#endif