#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>


struct Light{
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



#endif