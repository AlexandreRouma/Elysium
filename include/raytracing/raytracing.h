#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtx/intersect.hpp>
#include <vector>

#define OBJECT_TYPE_TRIANGLE    0
#define OBJECT_TYPE_SPHERE      1
#define OBJECT_TYPE_INVALID     0xFF

namespace rt {
    // Object specific data

    struct TriangleData {
        glm::vec3 a;
        glm::vec3 b;
        glm::vec3 c;
        glm::vec3 normal;
    };

    struct TriangleSurfaceCoord {
        glm::vec2 bari;
    };

    struct SphereData {
        glm::vec3 center;
        float radius;
    };

    struct SphereSurfaceCoord {
        glm::vec3 polar;
    };

    // General object data
    struct Texture {
        unsigned short width;
        unsigned short height;
        glm::vec3* data;
    };

    struct Material {
        glm::vec3 ambiantColor;
        glm::vec3 diffuseColor;
        glm::vec3 specularColor;
        glm::vec3 emissiveColor;
        glm::vec3 translucentColor;
        bool isEmissive;
        bool hasTexture;
        float emissionBrightness;
        float ambiantFactor;
        float diffuseFactor;
        float specularFactor;
        float shinyness;
        float translucency;
        Texture texture;
    };

    struct SurfaceCoord {
        union {
            TriangleSurfaceCoord triangle;
            SphereSurfaceCoord sphere;
        };
    };

    struct Object {
        unsigned char type;
        Material material;
        union {
            TriangleData triangleData;
            SphereData sphereData;
        };
    };

    struct Camera {
        glm::vec3 position;
        glm::mat4 rotationMat;
        float distance;
    };

    struct Scene {
        Camera camera;
        int objectCount;
        Object* objects;
    };

    struct Buffer {
        int width;
        int height;
        glm::vec3* pixels;
    };

    struct Ray {
        glm::vec3 direction;
        glm::vec3 origin;
    };

    // Functions
    Object createTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, Material material);
    Camera createCamera(glm::vec3 position, glm::vec2 rotation, float FOV);
    Buffer createBuffer(int width, int height);
    void clearBuffer(Buffer& buffer);
    void copyBuffer(Buffer& src, Buffer& dst);
    void destroyBuffer(Buffer& buffer);
    Scene compileScene(Camera camera, std::vector<Object> objects);
    void destroyScene(Scene& scene);
    void appendObjList(std::vector<Object>& objects, std::vector<Object>& list);
}