#ifndef RENDERER_H
#define RENDERER_H

#include <scene.hpp>
#include <mesh.hpp>
#include <kdtree.hpp>

#include <glm/glm.hpp>
#include <vector>

class Renderer {
public:
    Renderer(std::vector<Mesh>& mesh, Scene& _sceme);
    ~Renderer(){delete[] data;};
    void render(glm::vec3 origin, glm::vec3 dir, float FOV);
    uint8_t* getImage();
    std::vector<std::vector<glm::vec3>>* getImageTwo();
    void draw();
    void save();
private:
    glm::vec3 rayTrace(const Ray& ray);
    bool intersectShadow(const Ray& ray);
    bool intersectModel(const Ray& ray, glm::vec3& touch, glm::vec3& normal, Color& color);
    void calcScreen(glm::vec3 dir, float FOV, glm::vec3& corner, glm::vec3& down, glm::vec3& right);
    std::vector<Mesh>& meshes;
    Scene scene;
    std::vector < std::vector <glm::vec3> > image;
    uint8_t* data;

    KDTree kdtree;

    int width, height;
    float widthf, heightf;
    int rec_depth;
};


#endif