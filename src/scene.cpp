#include <scene.hpp>

#include <iostream>
#include <fstream>

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
        LightPoint tmpLight;
        uint8_t red, green, blue;
        file >> tmpLight.pos.x >> tmpLight.pos.y >> tmpLight.pos.z;
        file >> red >> green >> blue;
        file >> tmpLight.intensity;
        tmpLight.color = {(float)(red) / 255.0f, (float)(green) / 255.0f, (float)(blue) / 255.0f};
        lightPoint.push_back(tmpLight);
    }
    file.close();
}