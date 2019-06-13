#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

struct Material{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 emissive;
    float shininess = 32.0f;
    float refraction = 1.0f;
    float intensity = 1.0f;
    bool set;
};

#endif