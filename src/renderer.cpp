#include <renderer.hpp>

#include <glm/gtx/intersect.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <FreeImage.h>

#include <iostream>
#include <cstdlib>

#define RUSSIAN_ROULETE true

Renderer::Renderer(std::vector<Mesh>& mesh, Scene& _scene) : meshes(mesh), scene(_scene), gen(rd()), rand(0.0, 1.0){
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

bool Renderer::intersectModel(const Ray& ray, glm::vec3& touch, glm::vec3& normal, Material& material) {
    glm::vec3 baryPosition;
    int oldIndex;
    int meshIndex = kdtree.intersect(ray, baryPosition, oldIndex);
    if(meshIndex < 0)
        return false;
    Mesh& mesh = meshes[meshIndex];

    material = mesh.getMaterial();
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
    FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(scene.filename_out.c_str());
    FIBITMAP *bitmap;


    if (format == FIF_EXR || format == FIF_HDR) {
        // export in high dynamic range
        bitmap = FreeImage_AllocateT(FIT_RGBF, scene.scr_width, scene.scr_height);
        if (!bitmap) {
            std::cerr << "FreeImage export failed.\n";
            FreeImage_DeInitialise();
            return;
        }
    
        unsigned offset = FreeImage_GetPitch(bitmap);
        auto bits = FreeImage_GetBits(bitmap);

        for (int y = scene.scr_height; y-- > 0;) {
            float *pixel = reinterpret_cast<float *>(bits);
            for (int x = 0; x < scene.scr_width; x++) {
                *(pixel++) = image[y][x].r;
                *(pixel++) = image[y][x].g;
                *(pixel++) = image[y][x].b;
            }
            bits += offset;
        }
    } else {
        bitmap = FreeImage_Allocate(scene.scr_width, scene.scr_height, 24);
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
    }

    if (FreeImage_Save(format, bitmap, scene.filename_out.c_str(), 0))
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

    for(int y=0; y<height; y++) 
        for(int x=0; x<width; x++)
            image[y][x] = glm::vec3(0.0f);
    
    for(int samples=0; samples < scene.samples; samples++) {
        for(int y=0; y<height; y++) {
            // std::cout << y << "/" << height << std::endl; // PROGRESS BAR
            for(int x=0; x<width; x++) {
                glm::vec3& pixel = image[y][x];
                glm::vec3 direction = ((float(y) + float(rand(gen)) - 0.5f )/heightf)*down + ((float(x) + float(rand(gen)) - 0.5f)/widthf)*right;
                Ray ray(viewer, corner + direction);
                pixel += rayTrace(ray, 1);

                maxValue = pixel.x > maxValue ? pixel.x : maxValue;
                maxValue = pixel.y > maxValue ? pixel.y : maxValue;
                maxValue = pixel.z > maxValue ? pixel.z : maxValue;
            }
        }
        std::cout << samples;
        save();
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

glm::vec3 Renderer::rayTrace(const Ray& ray, int level) {
    glm::vec3 normal, touch;
    Material material;
    if(!intersectModel(ray, touch, normal, material))
        return {0.0f, 0.0f, 0.0f};

    glm::vec3 direct = (level == 1) ? material.emissive * M_1_PIf32 : glm::vec3(0.0f);
    
    // sample light
    int ltID = int(rand(gen) * float(scene.lightTriangle.size()) * 0.9999f);
    float u = rand(gen);
    float v = rand(gen);
    if(u + v > 1.0f) {
        std::swap(u, v);
        u = 1.0f - u; 
        v = 1.0f - v;
    }

    glm::vec3 lightTouch = scene.lightTriangle[ltID].pos[0] * (1.0f - u - v) +
                           scene.lightTriangle[ltID].pos[1] * u +
                           scene.lightTriangle[ltID].pos[2] * v;

    // glm::vec3 lightNormal = glm::cross(scene.lightTriangle[ltID].pos[1] - scene.lightTriangle[ltID].pos[0],
    //                                    scene.lightTriangle[ltID].pos[2] - scene.lightTriangle[ltID].pos[0]);

    // lightNormal = glm::normalize(lightNormal);

    glm::vec3 lightNormal = scene.lightTriangle[ltID].normal[0] * (1.0f - u - v) +
                            scene.lightTriangle[ltID].normal[1] * u +
                            scene.lightTriangle[ltID].normal[2] * v;

    glm::vec3 f_diffuse = material.diffuse * M_1_PIf32;

    glm::vec3 wl = lightTouch - touch; // direction to light
    
    // calc light factor 
    glm::vec3 lightFactor = glm::vec3(0.0f);
    Ray intoLight = Ray(touch + 0.0001f*normal, wl + 0.0001f * lightNormal);

    if(glm::dot(wl, normal) > 0.0f && !intersectShadow(intoLight)) {
        glm::vec3 nwl = glm::normalize(wl);

        float invPdf = float(scene.lightTriangle.size()) * scene.lightTriangle[ltID].surface;
        glm::vec3 Le = scene.lightTriangle[ltID].color;
        float geometric = std::max(0.0f, glm::dot(normal, nwl)) *
                        std::max(0.0f, glm::dot(lightNormal, -nwl)) /
                        (1.0f + glm::distance2(lightTouch, touch));

        lightFactor = f_diffuse * invPdf * Le * geometric;
    } 

    #if RUSSIAN_ROULETE
        float maxf = std::max(f_diffuse.r, f_diffuse.g);
        maxf = f_diffuse.b > maxf ? f_diffuse.b : maxf;
        maxf *= 1.5f;
        if (level > 8 || rand(gen) > maxf)
            return direct + lightFactor;
    #else
        if(level > 8)
            return direct + lightFactor;
    #endif

    // indirect factor
    glm::vec3 wi = glm::vec3(1.0f, 1.0f, 1.0f);
    while(glm::length2(wi) > 1.0f || glm::dot(wi, normal) - 0.001f < 0.0f) {
        wi = glm::vec3(rand(gen)*2.0f - 1.0f,
                       rand(gen)*2.0f - 1.0f,
                       rand(gen)*2.0f - 1.0f);
    }
    wi = glm::normalize(wi);    
    Ray wiRay(touch + 0.0001f * normal, wi);

    float cosine = glm::dot(normal, wi);
    float invPdf = M_PI * 2.0f;
    glm::vec3 indirectFactor = f_diffuse * invPdf * cosine * rayTrace(wiRay, level + 1);

    #if RUSSIAN_ROULETE
        return direct + lightFactor + indirectFactor / maxf;
    #else
        return direct + lightFactor + indirectFactor;
    #endif
}

bool Renderer::intersectShadow(const Ray& ray) {
    return kdtree.intersect(ray);
}