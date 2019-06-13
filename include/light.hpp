#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>


struct LightPoint{
    glm::vec3 color;
    glm::vec3 pos;
    float& r = color.r;
    float& g = color.g;
    float& b = color.b;
    float& x = pos.x;
    float& y = pos.y;
    float& z = pos.z;
    float intensity;
};

struct LightTriangle{
    glm::vec3 color;
    glm::vec3 pos[3];
    glm::vec3 normal[3];
    float& r = color.r;
    float& g = color.g;
    float& b = color.b;
    float intensity;
    float surface = 1.0f;
};


#endif