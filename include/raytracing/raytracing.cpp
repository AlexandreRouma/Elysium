#include <raytracing/raytracing.h>

namespace rt {
    Camera createCamera(glm::vec3 position, glm::vec2 rotation, float FOV) {
        Camera cam;
        cam.position = position;
        cam.rotationMat = glm::rotate(rotation.x, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(rotation.y, glm::vec3(1.0f, 0.0f, 0.0f));
        cam.distance = 0.5f / tan(glm::radians(FOV / 2.0f));
        return cam;
    }

    Object createTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, Material material) {
        Object obj;
        obj.type = OBJECT_TYPE_TRIANGLE;
        obj.material = material;
        obj.triangleData.a = a;
        obj.triangleData.b = b;
        obj.triangleData.c = c;
        obj.triangleData.normal = glm::triangleNormal(a, b, c);
        return obj;
    }

    Buffer createBuffer(int width, int height) {
        Buffer buf;
        buf.height = height;
        buf.width = width;
        buf.pixels = new glm::vec3[width * height];
        return buf;
    }

    void clearBuffer(Buffer& buffer) {
        for (int y = 0; y < buffer.height; y++) {
            for (int x = 0; x < buffer.height; x++) {
                buffer.pixels[(y * buffer.width) + x] = glm::vec3(0.0f, 0.0f, 0.0f);
            }
        }
    }

    void copyBuffer(Buffer& src, Buffer& dst) {
        if (src.width != dst.width || src.height != dst.height) {
            return;
        }
        memcpy(dst.pixels, src.pixels, src.width * src.height * sizeof(glm::vec3));
    }

    void destroyBuffer(Buffer& buffer) {
        delete[] buffer.pixels;
        buffer.width = 0;
        buffer.height = 0;
    }

    Scene compileScene(Camera camera, std::vector<Object> objects) {
        Scene scn;
        scn.camera = camera;
        scn.objectCount = objects.size();
        scn.objects = new Object[scn.objectCount];
        for (int i = 0; i < scn.objectCount; i++) {
            scn.objects[i] = objects[i];
        }
        return scn;
    }

    void destroyScene(Scene& scene) {
        delete[] scene.objects;
        scene.objectCount = 0;
    }

    void appendObjList(std::vector<Object>& objects, std::vector<Object>& list) {
        objects.insert(objects.end(), list.begin(), list.end());
    }
};