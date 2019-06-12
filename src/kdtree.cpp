#include <kdtree.hpp>



Triangle::Triangle(glm::vec3 _v1, glm::vec3 _v2, glm::vec3 _v3, uint index, uint _oldIndex)
    : v1(_v1), v2(_v2), v3(_v3), meshIndex(index), oldIndex(_oldIndex)
{}

// ################################################################

int Bounds3f::maxAxis(){
    glm::vec3 size;
    for(int i=0; i<3; i++)
        size[i] = high[i] - low[i];
    if(size[0] > size[1] && size[0] > size[2])
        return 0;
    else if(size[1] > size[0] && size[1] > size[2])
        return 1;
    return 2;
}

Bounds3f Bounds3f::unionBounds(Bounds3f A){
    for(int i=0; i<3; i++) {
        if(low[i] < A.low[i])
            A.low[i] = low[i];
        if(high[i] > A.high[i])
            A.high[i] = high[i];
    }
    return A;
};

bool Bounds3f::intersectRay(const Ray ray, float& tmin, float& tmax) {
    float tymin, tymax, tzmin, tzmax;
 
    if (ray.invDir.x >= 0) { 
        tmin = (low.x - ray.orig.x) * ray.invDir.x; 
        tmax = (high.x - ray.orig.x) * ray.invDir.x; 
    } 
    else { 
        tmin = (high.x - ray.orig.x) * ray.invDir.x; 
        tmax = (low.x - ray.orig.x) * ray.invDir.x; 
    } 
    if (ray.invDir.y >= 0) { 
        tymin = (low.y - ray.orig.y) * ray.invDir.y; 
        tymax = (high.y - ray.orig.y) * ray.invDir.y; 
    } 
    else { 
        tymin = (high.y - ray.orig.y) * ray.invDir.y; 
        tymax = (low.y - ray.orig.y) * ray.invDir.y; 
    } 
 
    if ((tmin > tymax) || (tymin > tmax)) 
        return false; 
    if (tymin > tmin) 
        tmin = tymin; 
    if (tymax < tmax) 
        tmax = tymax; 
 
    if (ray.invDir.z >= 0) { 
        tzmin = (low.z - ray.orig.z) * ray.invDir.z; 
        tzmax = (high.z - ray.orig.z) * ray.invDir.z; 
    } 
    else { 
        tzmin = (high.z - ray.orig.z) * ray.invDir.z; 
        tzmax = (low.z - ray.orig.z) * ray.invDir.z; 
    } 
 
    if ((tmin > tzmax) || (tzmin > tmax)) 
        return false; 
    if (tzmin > tmin) 
        tmin = tzmin; 
    if (tzmax < tmax) 
        tmax = tzmax; 
 
    return true; 
}

Bounds3f Bounds3f::bounds(Triangle t) {
    Bounds3f bound;
    for(int i=0; i<3; i++) {
        if(t.v1[i] < t.v2[i] && t.v1[i] < t.v3[i])
            bound.low[i] = t.v1[i];
        if(t.v2[i] < t.v1[i] && t.v2[i] < t.v3[i])
            bound.low[i] = t.v2[i];
        if(t.v3[i] < t.v1[i] && t.v3[i] < t.v2[i])
            bound.low[i] = t.v3[i];

        if(t.v1[i] > t.v2[i] && t.v1[i] > t.v3[i])
            bound.high[i] = t.v1[i];
        if(t.v2[i] > t.v1[i] && t.v2[i] > t.v3[i])
            bound.high[i] = t.v2[i];
        if(t.v3[i] > t.v1[i] && t.v3[i] > t.v2[i])
            bound.high[i] = t.v3[i];
    }
    return bound;
}

// ################################################################

bool KDNode::isLeaf() const {return (child & 3) == 3;};
int KDNode::splitAxis() const {return child & 3;};
int KDNode::numberOfPrimitives() const {return number >> 2;};
int KDNode::aboveChild() const {return child >> 2;}; // > 0
int KDNode::belowChild() const {return (child >> 2) + 1;}; // <= 0
int KDNode::offset() const {return primitiveOffset;};
float KDNode::split() const {return splitValue;};

void KDNode::initLeaf(int _offset, int _number) {
    primitiveOffset = _offset;
    number = (_number << 2) | 3;
}
void KDNode::initNode(int _axis, int _aboveChild, float _split) {
    splitValue = _split;
    child = (_aboveChild << 2) | (_axis & 3);
}

// ################################################################

void KDTree::init(std::vector<Mesh> const& mesh) {
    std::vector<Triangle> world;
    for(uint m=0; m<mesh.size(); m++){
        //populate world using mesh
        for(uint i=0;i<mesh[m].indices.size(); i+=3) {
            world.push_back(Triangle(
                mesh[m].vertices[mesh[m].indices[i]].Position,
                mesh[m].vertices[mesh[m].indices[i+1]].Position,
                mesh[m].vertices[mesh[m].indices[i+2]].Position,
                m,
                i));
        }
    }
    worldBounds = Bounds3f::bounds(world[0]);
    for(auto& triangle : world) {
        worldBounds = worldBounds.unionBounds(Bounds3f::bounds(triangle));
    }
    tree.clear();
    tree.push_back(KDNode());
    buildTree(0, world, worldBounds, 0);
};

void KDTree::buildTree(int nodeIndex, std::vector<Triangle> set, Bounds3f bound, int rec_depth) {
    int axis;
    float split;
    std::vector <Triangle> belowSet, aboveSet;

    // if(set.size() <= MAX_LEAF_SIZE || rec_depth > std::round(8 + 1.3f * std::log2f(worldSize))) {
    if(set.size() <= MAX_LEAF_SIZE || !splitSets(set, bound, belowSet, aboveSet, axis, split)) {
        tree[nodeIndex].initLeaf(primitives.size(), set.size());
        // primitives.insert(primitives.end(), set.begin(), set.end());
        for(uint i=0; i<set.size(); i++) {
            primitives.push_back(set[i]);
        }
        return;
    }
    int aboveChild = tree.size();
    int belowChild = tree.size()+1;
    tree.resize(tree.size() + 2);
    tree[nodeIndex].initNode(axis, aboveChild, split);
    

    Bounds3f belowBound = bound;
    Bounds3f aboveBound = bound;
    belowBound.high[axis] = split;
    aboveBound.low[axis] = split;

    
    buildTree(aboveChild, aboveSet, aboveBound, rec_depth + 1);
    buildTree(belowChild, belowSet, belowBound, rec_depth + 1);
    return;
}

// bool KDTree::splitSets(std::vector<Triangle> set, Bounds3f bound,
//                        std::vector<Triangle>& belowSet, std::vector<Triangle>& aboveSet,
//                        int& axis, float& split) {
//     axis = bound.maxAxis();
//     split = (bound.low[axis] + bound.high[axis]) / 2.0f;

//     for(auto& triangle : set) {
//         if(triangle.v1[axis] < split || triangle.v2[axis] < split || triangle.v3[axis] < split)
//             belowSet.push_back(triangle);
//         if(triangle.v1[axis] > split || triangle.v2[axis] > split || triangle.v3[axis] > split)
//             aboveSet.push_back(triangle);
//     }
//     if(aboveSet.size() >= set.size() || belowSet.size() >= set.size())
//         return false;
//     return true;
// }


bool KDTree::splitSets(std::vector<Triangle> set, Bounds3f bound,
                       std::vector<Triangle>& belowSet, std::vector<Triangle>& aboveSet,
                       int& axis, float& split) {
    int maxAxis = bound.maxAxis();
    float cost = (bound.low[maxAxis] + bound.high[maxAxis]) / 2.0f;
    axis = 3;
    split = 0.0f;

    for(int tmpAxis = 0; tmpAxis < 3; tmpAxis++) {
        for(float ratio=0.01f; ratio < 1.0f; ratio += 0.01f) {
            float tmpSplit = bound.low[tmpAxis] + ratio * bound.high[tmpAxis];
            float tmpCost = 0.0f;
            size_t belowSize = 0;
            size_t aboveSize = 0;

            for(auto& triangle : set) {
                if(triangle.v1[tmpAxis] < tmpSplit || triangle.v2[tmpAxis] < tmpSplit || triangle.v3[tmpAxis] < tmpSplit) {
                    tmpCost += ratio;
                    belowSize++;
                }
                if(triangle.v1[tmpAxis] > tmpSplit || triangle.v2[tmpAxis] > tmpSplit || triangle.v3[tmpAxis] > tmpSplit) {
                    tmpCost += (1.0f - ratio);
                    aboveSize++;
                }
            }
            if (belowSize < set.size() && aboveSize < set.size() && tmpCost < cost) {
                axis = tmpAxis;
                split = tmpSplit;
                cost = tmpCost;
            }
        }
    }

    if(axis == 3)
        return false;

    for(auto& triangle : set) {
        if(triangle.v1[axis] < split || triangle.v2[axis] < split || triangle.v3[axis] < split)
            belowSet.push_back(triangle);
        if(triangle.v1[axis] > split || triangle.v2[axis] > split || triangle.v3[axis] > split)
            aboveSet.push_back(triangle);
    }
    // if(aboveSet.size() >= set.size() || belowSet.size() >= set.size())
    
    return true;
}


// KDTree::KDNode::Split KDTree::findSplit(std::vector<Triangle> &tris, glm::vec3 &max, glm::vec3 &min) {
//     id_t bestAxis = 3; // if 3 is returned then there's no point in splitting
//     float bestSplit = 0.f;
//     float bestCost = float(tris.size());

//     for (id_t axis = 0; axis < 3; axis++) {
//         for (float ratio = 0.01f; ratio < 1.0f; ratio += 0.01f) {
//             float split = min[axis] + ratio * (max[axis] - min[axis]);
//             float cost = 0.f;
//             size_t leftCount = 0;
//             size_t rightCount = 0;

//             for (auto &tri : tris) {
//                 if (inLeft(tri, split, axis)) {
//                     cost += ratio;
//                     leftCount++;
//                 }
//                 if (inRight(tri, split, axis)) {
//                     cost += (1.f - ratio);
//                     rightCount++;
//                 }
//             }
//             if (leftCount < tris.size() && rightCount < tris.size() && cost < bestCost) {
//                 bestAxis = axis;
//                 bestSplit = split;
//                 bestCost = cost;
//             }
//         }
//     }

//     return {bestAxis, bestSplit};
// }




bool KDTree::intersect(const Ray& ray) {
    float tMin, tMax;
    if(!worldBounds.intersectRay(ray, tMin, tMax))
        return false;
    tMin = tMin < 0.0f ? 0.0f : tMin;
    tMax = tMax > 0.999f ? 0.999f : tMax;
    return intersectNode(0, ray, tMin, tMax);
}

// Returns mesh index / oldIndex - index of indices;
int KDTree::intersect(const Ray& ray, glm::vec3& baryPosition, int& oldIndex) {
    float tMin, tMax;
    if(!worldBounds.intersectRay(ray, tMin, tMax))
        return -1;
    tMin = tMin < 0.001f ? 0.001f : tMin;
    int triangleIndex = intersectNode(0, ray, tMin, tMax, baryPosition);
    if(triangleIndex < 0)
        return -2;
    
    oldIndex = primitives[triangleIndex].oldIndex;
    return primitives[triangleIndex].meshIndex;
}

bool KDTree::intersectNode(int nodeIndex,const Ray& ray, float tMin, float tMax) {
    if(tree[nodeIndex].isLeaf()) {
        for(int offset = tree[nodeIndex].offset(); offset < tree[nodeIndex].offset() + tree[nodeIndex].numberOfPrimitives(); offset++) {
            glm::vec3 result;
            if(intersectTriangle(ray, offset, result) && result.z + 0.001f < tMax && result.z - 0.001f  > tMin) {
                return true;
            }
        }
        return false;
    }

    // SPRAWDZ TO

    int axis = tree[nodeIndex].splitAxis();
    float tSplit = (tree[nodeIndex].split() - ray.orig[axis]) * ray.invDir[axis];

    int farChildIndex = tree[nodeIndex].aboveChild();
    int nearChildIndex = tree[nodeIndex].belowChild();
    if((ray.orig[axis] + ray.dir[axis]*tMin) > tree[nodeIndex].split())
        std::swap(nearChildIndex, farChildIndex);
    
    if(intersectNode(nearChildIndex, ray, tMin, tSplit))
        return true;
    return intersectNode(farChildIndex, ray, tSplit, tMax);
}

// return triangle index;
int KDTree::intersectNode(int nodeIndex, const Ray& ray, float tMin, float tMax, glm::vec3& baryPosition) {
    if(tree[nodeIndex].isLeaf()) {
        int triangleIndex = -1;
        baryPosition.z = std::numeric_limits<float>::infinity();
        for(int offset = tree[nodeIndex].offset(); offset < tree[nodeIndex].offset() + tree[nodeIndex].numberOfPrimitives(); offset++) {
            glm::vec3 result;
            if(intersectTriangle(ray, offset, result) && result.z < baryPosition.z
                                                              && result.z + 0.001f > tMin && result.z - 0.001f < tMax) {
                baryPosition = result;
                triangleIndex = offset;
            }
        }
        return triangleIndex;
    }

    // SPRAWDZ TO

    int axis = tree[nodeIndex].splitAxis();
    float tSplit = (tree[nodeIndex].split() - ray.orig[axis]) * ray.invDir[axis];

    int farChildIndex = tree[nodeIndex].aboveChild();
    int nearChildIndex = tree[nodeIndex].belowChild();
    if((ray.orig[axis] + ray.dir[axis]*tMin) > tree[nodeIndex].split())
        std::swap(nearChildIndex, farChildIndex);
    
    int nearTriangle = intersectNode(nearChildIndex, ray, tMin, tSplit, baryPosition);
    if(nearTriangle < 0) {
        return intersectNode(farChildIndex, ray, tSplit, tMax, baryPosition);
    }
    return nearTriangle;
}

bool KDTree::intersectTriangle(const Ray& ray, uint32_t offset, glm::vec3& result) {
    return glm::intersectRayTriangle(ray.orig, ray.dir, primitives[offset].v1, primitives[offset].v2, primitives[offset].v3, result);
}