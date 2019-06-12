#include <renderer.hpp>

#include <glm/gtx/intersect.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <FreeImage.h>

#include <iostream>


Renderer::Renderer(std::vector<Mesh>& mesh, Scene& _scene) : meshes(mesh), scene(_scene){
    width = scene.scr_width;
    height = scene.scr_height;
    widthf = (float)scene.scr_width;
    heightf = (float)scene.scr_height;
    rec_depth = scene.rec_depth;

    data = new uint8_t[3 * width * height];

    image.clear();
    for(int y=0; y<height; y++) {
        std::vector <glm::vec3> tmp;
        for(int x=0; x<width; x++) {
            tmp.push_back({0.0f, 0.0f, 0.0f});
        }
        image.push_back(tmp);
    }

    // ####### KDTREE ########
    kdtree.init(mesh);
}

bool Renderer::intersectModel(const Ray& ray, glm::vec3& touch, glm::vec3& normal, Color& color) {
    glm::vec3 baryPosition;
    int oldIndex;
    int meshIndex = kdtree.intersect(ray, baryPosition, oldIndex);
    if(meshIndex < 0)
        return false;
    Mesh& mesh = meshes[meshIndex];

    color = mesh.getColor();
    Vertex& vec1  = mesh.vertices[mesh.indices[oldIndex    ]];
    Vertex& vec2  = mesh.vertices[mesh.indices[oldIndex + 1]];
    Vertex& vec3  = mesh.vertices[mesh.indices[oldIndex + 2]];
    normal = vec1.Normal * (1.0f - baryPosition.x - baryPosition.y) + vec2.Normal * baryPosition.x + vec3.Normal * baryPosition.y;
    touch = ray.orig + baryPosition.z * ray.dir;
    return true;
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

void Renderer::save() {
    FreeImage_Initialise();

    FIBITMAP *bitmap = FreeImage_Allocate(scene.scr_width, scene.scr_height, 24);
    if (!bitmap) {
        std::cerr << "FreeImage export failed.\n";
        FreeImage_DeInitialise();
        return;
    }

    RGBQUAD color;
    for (int y = scene.scr_height - 1, i = 0; y >= 0; y--) {
        for (int x = 0; x < scene.scr_width; x++) {
            color.rgbRed = data[i++];
            color.rgbGreen = data[i++];
            color.rgbBlue = data[i++];
            FreeImage_SetPixelColor(bitmap, x, y, &color);
        }
    }
    if (FreeImage_Save(FreeImage_GetFIFFromFilename(scene.filename_out.c_str()), bitmap, scene.filename_out.c_str(), 0))
        std::cerr << "Render succesfully saved to file " << scene.filename_out << "\n";
    else
        std::cerr << "FreeImage export failed.\n";

    FreeImage_DeInitialise();
}

void Renderer::calcScreen(glm::vec3 dir, float FOV, glm::vec3& corner, glm::vec3& down, glm::vec3& right){
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

void Renderer::render(glm::vec3 viewer, glm::vec3 dir, float FOV){
    glm::vec3 corner;
    glm::vec3 down;
    glm::vec3 right;
    calcScreen(dir, FOV, corner, down, right);

    float maxValue = 0.0f;
    for(int y=0; y<height; y++) {
        std::cout << y << "/" << height << std::endl; // PROGRESS BAR
        for(int x=0; x<width; x++) {
            glm::vec3& pixel = image[y][x];
            Ray ray(viewer, corner + (float(y)/heightf)*down + (float(x)/widthf)*right);
            pixel = rayTrace(ray);

            maxValue = pixel.x > maxValue ? pixel.x : maxValue;
            maxValue = pixel.y > maxValue ? pixel.y : maxValue;
            maxValue = pixel.z > maxValue ? pixel.z : maxValue;
        }
    }

    for(int y=0; y<height; y++) {
        for(int x=0; x<width; x++) {
            glm::vec3& pixel = image[y][x];
            data[y*width*3 + 3*x] = uint8_t(pixel.r * 255.0f / maxValue);
            data[y*width*3 + 3*x + 1] = uint8_t(pixel.g * 255.0f / maxValue);
            data[y*width*3 + 3*x + 2] = uint8_t(pixel.b * 255.0f / maxValue);
        }
    }
}

glm::vec3 Renderer::rayTrace(const Ray& ray) {
    glm::vec3 normal, touch;
    Color color;
    if(!intersectModel(ray, touch, normal, color))
        return {0.0f, 0.0f, 0.0f};
    
    // return color.diffuse;


    glm::vec3 diffuse = {0.0f, 0.0f, 0.0f};
    glm::vec3 specular = {0.0f, 0.0f, 0.0f};

    for(auto& light : scene.light) {
        Ray shadowRay(touch + 0.0001f * normal, light.pos);
        if(!intersectShadow(shadowRay)) {
            glm::vec3 source = glm::normalize(light.pos - touch);
            glm::vec3 reflected = glm::normalize(2.0f*glm::dot(source, normal)*normal - source);
            glm::vec3 viewer = glm::normalize(ray.orig - touch);

            float intensity = light.intensity * 1 / glm::distance2(touch, light.pos);

            diffuse += glm::dot(source, normal) * light.color * intensity;
            specular += glm::pow(glm::dot(reflected, viewer), color.alfa) * light.color * intensity;
        } else
            color.ambient = {0.0f, 0.3f, 0.7f};
    }
    return 0.1f * color.ambient  +  diffuse * color.diffuse  +  specular * color.specular;
}


bool Renderer::intersectShadow(const Ray& ray) {
    return kdtree.intersect(ray);
}










    // for (auto& mesh : meshes) {
    //     auto& indices = mesh.indices;
    //     auto& vertices = mesh.vertices;
    //     glm::vec3 result;

    //     for(int i=0; i<indices.size(); i+=3) {
    //         Vertex& vec1 = vertices[indices[i]];
    //         Vertex& vec2 = vertices[indices[i+1]];
    //         Vertex& vec3 = vertices[indices[i+2]];
    //         if(glm::intersectRayTriangle(origin, ray, vec1.Position, vec2.Position, vec3.Position, result)){
    //             return true;
    //         }
    //     }
    // }
    // return false;

/*
// Returns index of intersected triangle
int intersectWorld(const Ray& ray, glm::vec3& result, ) {
    float distance = FLT_MAX;
    int index;
    for (auto& mesh : meshes) {
        auto& indices = mesh.indices;
        auto& vertices = mesh.vertices;

        for(int i=0; i<indices.size(); i+=3) {
            Vertex& vec1 = vertices[indices[i]];
            Vertex& vec2 = vertices[indices[i+1]];
            Vertex& vec3 = vertices[indices[i+2]];
            glm::vec3 tmp;
            if(glm::intersectRayTriangle(origin, ray, vec1.Position, vec2.Position, vec3.Position, tmp)){
                if(result.z < distance) {
                    distance = tmp.z;
                    result = tmp; /// Sprawdz typy i resturnowanie
                }
            }
        }
    }
    return false;
}
*/