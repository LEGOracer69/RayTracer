#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>

struct Ray {
    Ray(const glm::vec3 &_origin, const glm::vec3 &_dir) : orig(_origin), dir(_dir) 
    { 
        invDir.x = 1.0f / dir.x;
        invDir.y = 1.0f / dir.y;
        invDir.z = 1.0f / dir.z;
    } 
    glm::vec3 orig, dir;       // ray orig and dir 
    glm::vec3 invDir; 
    
    int sign(int axis) {
        return invDir[axis] < 0;
    }; 
};


#endif