#ifndef SCENE_H
#define SCENE_H

#include <glm/glm.hpp>

#include <light.h>
#include <camera.h>

#include <iostream>
#include <fstream>
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

    glm::vec3 VP;
    glm::vec3 LA;
    glm::vec3 UP;
    
    float yview;
    std::vector <Light> light;
};

Scene::Scene(std::string filename) {
    std::ifstream file(filename);
    std::string comment;
    std::getline(file, comment);
    file >> filename_in >> filename_out;
    file >> rec_depth >> scr_width >> scr_height;
    file >> VP.x >> VP.y >> VP.z;
    file >> LA.x >> LA.y >> LA.z;
    file >> UP.x >> UP.y >> UP.z;
    file >> yview;

    char c;
    while (file >> c) {
        if (c != 'L') {
            file.unget();
            break;
        }
        Light tmpLight;
        uint8_t red, green, blue;
        file >> tmpLight.pos.x >> tmpLight.pos.y >> tmpLight.pos.z;
        file >> red >> green >> blue;
        file >> tmpLight.intensity;
        tmpLight.color = {(float)(red) / 255.0f, (float)(green) / 255.0f, (float)(blue) / 255.0f};
        light.push_back(tmpLight);
    }
    file.close();
}

#endif