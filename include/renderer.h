#ifndef RENDERER_H
#define RENDERER_H

#include <mesh.h>

struct Ray {
    glm::vec3 o;
    glm::vec3 dir;
};

class Renderer {
public:
    Renderer(std::vector<Mesh>& mesh, int width, int height, int depth);
    ~Renderer(){delete[] data;};
    void render(glm::vec3 origin, glm::vec3 dir, float FOV);
    uint8_t* getImage();
    std::vector<std::vector<glm::vec3>>* getImageTwo();
    void draw();
private:
    void rayTrace();
    void calcScreen(glm::vec3 origin, glm::vec3 dir, float FOV, glm::vec3& corner, glm::vec3& down, glm::vec3& right);
    std::vector<Mesh>& meshes;
    std::vector < std::vector <glm::vec3> > image;
    uint8_t* data;

    int width, height;
    float widthf, heightf;
    int rec_depth;
};

Renderer::Renderer(std::vector<Mesh>& mesh, int scr_width, int scr_height, int depth) : meshes(mesh){
    width = scr_width;
    height = scr_height;
    widthf = (float)scr_width;
    heightf = (float)scr_height;
    rec_depth = depth;

    data = new uint8_t[3 * scr_width * scr_height];

    image.clear();
    for(int y=0; y<height; y++) {
        std::vector <glm::vec3> tmp;
        for(int x=0; x<width; x++) {
            tmp.push_back({0.0f, 0.0f, 0.0f});
        }
        image.push_back(tmp);
    }
}

uint8_t* Renderer::getImage(){
    /*
    for(int y=0; y<height; y++) {
        for(int x=0; x<width; x++) {
            data[y*width*3 + 3*x] = (uint8_t)(image[y][x].r * 255.0f);
            data[y*width*3 + 3*x + 1] = (uint8_t)(image[y][x].g * 255.0f);
            data[y*width*3 + 3*x + 2] = (uint8_t)(image[y][x].b * 255.0f);
        }
    }
    */
    return data;
}

std::vector<std::vector<glm::vec3>>* Renderer::getImageTwo(){
    return &image;
}

void Renderer::draw() {
    std::cout << "P3\n60 30\n255\n";
    for(int i=0; i<3*width*height; i++) {
        std::cout << (uint32_t)data[i] << " ";
    }
}

void Renderer::calcScreen(glm::vec3 origin, glm::vec3 dir, float FOV, glm::vec3& corner, glm::vec3& down, glm::vec3& right){
    float near = 1.0f;

    float yoffset = near * tanf(FOV * (3.141592653589793 / 360.0));
    float xoffset = yoffset * ( widthf / heightf );

    corner = {-xoffset, yoffset, -near};
    down = {0.0f, -2.0f * yoffset, 0.0f};
    right = {2.0f * xoffset, 0.0f, 0.0f};

    glm::vec3 eye = {0.0f, 0.0f, 0.0f};
    glm::vec3 upp = {0.0f, 1.0f, 0.0f};
    glm::mat3 rotate = glm::inverse(glm::mat3(glm::lookAt(eye, dir, upp)));

    corner = rotate * corner;
    down = rotate * down;
    right = rotate * right;
}


/*
void Renderer::calcScreen(glm::vec3 origin, glm::vec3 dir, float FOV, glm::vec3& corner, glm::vec3& down, glm::vec3& right){
    double near = 1.0;

    double yoffset = near * tan(double(FOV)/ 2.0);
    double xoffset = yoffset * ( double(widthf) / double(heightf) );
    
    corner = {(float)-xoffset, (float)yoffset, (float)-near};
    down = {0.0f, -2.0f * (float)yoffset, 0.0f};
    right = {2.0f * (float)xoffset, 0.0f, 0.0f};

    glm::vec3 eye = {0.0f, 0.0f, 0.0f};
    glm::vec3 upp = {0.0f, 1.0f, 0.0f};
    glm::mat3 rotate = glm::inverse(glm::mat3(glm::lookAt(eye, dir, upp)));

    corner = rotate * corner;
    down = rotate * down;
    right = rotate * right;
}
*/

void Renderer::render(glm::vec3 origin, glm::vec3 dir, float FOV){
    glm::vec3 corner;
    glm::vec3 down;
    glm::vec3 right;
    calcScreen(origin, dir, FOV, corner, down, right);

    for(int y=0; y<height; y++) {
        for(int x=0; x<width; x++) {
            data[y*width*3 + 3*x] = 0;
            data[y*width*3 + 3*x + 1] = 0;
            data[y*width*3 + 3*x + 2] = 0;
            image[y][x] = {0.0f, 0.0f, 0.0f};
            glm::vec3 ray = corner + (float(y)/heightf)*down + (float(x)/widthf)*right;


            for (auto mesh : meshes) {
                auto& indices = mesh.indices;
                auto& vertices = mesh.vertices;
                for(int i=0; i<indices.size(); i+=3) {
                    glm::vec3 vec1 = vertices[indices[i]].Position;
                    glm::vec3 vec2 = vertices[indices[i+1]].Position;
                    glm::vec3 vec3 = vertices[indices[i+2]].Position;
                    glm::vec3 result;

                    if(glm::intersectRayTriangle(origin, ray, vec1, vec2, vec3, result)){
                        data[y*width*3 + 3*x] = uint8_t(mesh.color.diffuse.r * 255.0f);
                        data[y*width*3 + 3*x + 1] = uint8_t(mesh.color.diffuse.g * 255.0f);
                        data[y*width*3 + 3*x + 2] = uint8_t(mesh.color.diffuse.b * 255.0f);
                        image[y][x] = mesh.color.diffuse;
                        /*
                        data[y*width*3 + 3*x] = 255;
                        data[y*width*3 + 3*x + 1] = 255;
                        data[y*width*3 + 3*x + 2] = 255;
                        image[y][x] = {1.0f, 1.0f, 1.0f};
                        */
                        break;
                    }
                }
            }
        }
    }
    
}








#endif