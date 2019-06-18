#ifndef KDTREE_H
#define KDTREE_H
#include <mesh.hpp>
#include <ray.hpp>

#include <glm/glm.hpp>
#include <vector>
#include <glm/gtx/intersect.hpp>

#include <iostream>

#define MAX_LEAF_SIZE 8

struct Triangle {
    glm::vec3 v1;
    glm::vec3 v2;
    glm::vec3 v3;
    uint meshIndex;
    uint oldIndex; // Index in indices

    Triangle(glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _v3, uint index, uint _oldIndex);
};

struct Bounds3f {
    glm::vec3 low;
    glm::vec3 high;

    int maxAxis();
    Bounds3f unionBounds(Bounds3f A);
    bool intersectRay(const Ray ray, float& tmin, float& tmax);
    static Bounds3f bounds(Triangle t);
};

class KDNode {
    union {
        float splitValue; // node
        int primitiveOffset; // leaf
    };
    union {
        int child; //  node - first child
        int number; // leaf - number of primitives stored
    };

public:
    bool isLeaf() const;
    int splitAxis() const;
    int numberOfPrimitives() const;
    int aboveChild() const;
    int belowChild() const;
    int offset() const;
    float split() const;

    void initLeaf(int _offset, int _number);
    void initNode(int _axis, int _aboveChild, float _split);
};

class KDTree {
public:
    bool intersect(const Ray& ray);
    int intersect(const Ray& ray, glm::vec3& baryPosition, int& oldIndex); //Returns mesh indexcc
    void init(std::vector<Mesh> const& mesh);
private:
    bool intersectNode(int nodeIndex,const Ray& ray, float tMin, float tMax);
    int intersectNode(int nodeIndex, const Ray& ray, float tMin, float tMax, glm::vec3& baryPosition); //returns traingle index
    bool intersectTriangle(const Ray& ray, uint32_t offset, glm::vec3& result);
    std::vector <KDNode> tree;
    void buildTree(int node, std::vector<Triangle> set, Bounds3f bound, int rec_depth);
    bool splitSets(std::vector<Triangle> set, Bounds3f bound,
                   std::vector<Triangle>& belowSet, std::vector<Triangle>& aboveSet,
                   int& axis, float& split);

    std::vector <Triangle> primitives;
    Bounds3f worldBounds;
    uint32_t worldSize;
};

#endif // KDTREE_H