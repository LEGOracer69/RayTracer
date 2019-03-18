#ifndef COLOR_H
#define COLOR_H

#include <glm/glm.hpp>

struct Color{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    bool set;
};

#endif