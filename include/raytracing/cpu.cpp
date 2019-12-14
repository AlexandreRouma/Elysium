#include <raytracing/cpu.h>

#define M_PI            3.14159265358979323846
#define M_2_PI          2*M_PI

namespace rt {
    float cRand() {
        return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    }

    glm::vec3 CosineBRDF(glm::vec3 normal) {
        float r0 = cRand();
        float r1 = cRand();

        glm::vec3 uu = glm::normalize(glm::cross(normal, { 0.0f, 1.0f, 1.0f }));
        glm::vec3 vv = glm::cross(uu, normal);

        float ra = sqrtf(r1);

        float rx = ra * cosf(M_2_PI * r0);
        float ry = ra * sinf(M_2_PI * r0);

        float rz = sqrtf(1.0f - r1);

        return glm::normalize(rx * uu + ry * vv + rz * normal);
    }

    bool intersect(Ray ray, Scene scene, int& objectId, float& distance, SurfaceCoord& surfaceCoord, glm::vec3& normal) {
        objectId = -1;
        distance = 1000; // Set to INFINITY after verify it works
        float dist = 0.0f;
        SurfaceCoord surfCoord;
        bool intersect = false;
        for (int i = 0; i < scene.objectCount; i++) {
            if (scene.objects[i].type == OBJECT_TYPE_TRIANGLE) {
                intersect = glm::intersectRayTriangle(ray.origin, ray.direction, scene.objects[i].triangleData.a,
                                                scene.objects[i].triangleData.b, scene.objects[i].triangleData.c,
                                                surfCoord.triangle.bari, dist);
                if (intersect && dist > 0 && dist < distance) {
                    distance = dist;
                    surfaceCoord = surfCoord;
                    objectId = i;
                }
            }
            else {
                // BRUH DIS SHIT INVALID
            }
        }
        if (objectId > -1) {
            if (scene.objects[objectId].type == OBJECT_TYPE_TRIANGLE) {
                normal = scene.objects[objectId].triangleData.normal;
                if (glm::dot(normal, ray.direction) > 0) {
                    normal = -normal;
                }
            }
            else {
                // BRUH DIS SHIT INVALID
            }
            return true;
        }
        return false;
    }

    void probeRay(Ray ray, Scene& scene, glm::vec3& albedo, glm::vec3& normal, float& distance) {
        int id = 0;
        float dist = 0.0f;
        SurfaceCoord surfaceCoord;
        if (!intersect(ray, scene, id, dist, surfaceCoord, normal)) {
            distance = -1.0f;
            albedo = glm::vec3(0.0f, 0.0f, 0.0f);
            normal = glm::vec3(0.0f, 0.0f, 0.0f);
            return;
        }

        if (scene.objects[id].material.isEmissive) {
            albedo = scene.objects[id].material.emissiveColor;
        }
        else {
            albedo = scene.objects[id].material.diffuseColor;
        }
    }

    glm::vec3 castRay(Ray ray, Scene& scene, int maxBounce, int& bounceCount, float& distance) {
        int id = 0;
        float dist = 0.0f;
        SurfaceCoord surfaceCoord;
        glm::vec3 normal;
        glm::vec3 originalDir = ray.direction;
        
        if (bounceCount > maxBounce) {
            return glm::vec3(0.0f, 0.0f, 0.0f);
        }
        if (!intersect(ray, scene, id, dist, surfaceCoord, normal)) {
            distance += -1.0f;
            bounceCount++;
            return glm::vec3(0.0f, 0.0f, 0.0f); // sky color
        }


        // TODO: Add texture support

        float select = 0.9f;//cRand();
        Material mat = scene.objects[id].material;
        if (mat.isEmissive) {
            distance += dist;
            bounceCount++;
            return mat.emissiveColor * mat.emissionBrightness;
        }

        distance += dist;
        bounceCount++;
        ray.origin = (ray.direction * dist) + ray.origin + (normal * 0.00001f); // TODO: change epsilon value
        ray.direction = CosineBRDF(normal);
        return castRay(ray, scene, maxBounce, bounceCount, distance) * mat.diffuseColor;

        // if (select < mat.ambiantFactor) { // Ambiant
        //     printf("Ambiant!\n");
        //     distance += dist;
        //     bounceCount++;
        //     return scene.objects[id].material.ambiantColor;
        // }
        // else if (select < mat.ambiantFactor + mat.diffuseFactor) { // Diffuse
        //     distance += dist;
        //     bounceCount++;
        //     ray.origin = (ray.direction * dist) + ray.origin + (normal * 0.001f); // TODO: change epsilon value
        //     ray.direction = CosineBRDF(normal);
        //     return castRay(ray, scene, maxBounce, bounceCount, distance) * mat.diffuseColor;
        // }
        // else if (select < mat.ambiantFactor + mat.diffuseFactor + mat.specularFactor) { // Specular
        //     printf("Specular!\n");
        //     distance += dist;
        //     bounceCount++;
        //     ray.direction = glm::normalize(glm::reflect(ray.direction, normal));
        //     ray.origin = (originalDir * dist) + ray.origin + (ray.direction * 0.00001f); // TODO: change epsilon value
        //     return castRay(ray, scene, maxBounce, bounceCount, distance) * mat.specularColor;
        // }
        // else { // Translucent
        //     printf("Translucent! %f\n", select);
        //     distance += dist;
        //     bounceCount++;
        //     return glm::vec3(0.0f, 0.0f, 0.0f);
        // }
    }

    void cpuRender(Scene scene, Buffer buffer, int maxBounce) {
        float increment = 1.0f / (float)buffer.width;
        float fw = (increment * (float)buffer.height) / 2;
        #pragma omp parallel for schedule(dynamic)
        for (int y = 0; y < buffer.height; y++) {
            for (int x = 0; x < buffer.width; x++) {
                float xpos = ((float)x * increment) - 0.5f;
                float ypos = -(((float)y * increment) - fw);
                if (xpos < 0) {
                    xpos += increment / 2.0f;
                }
                else {
                    xpos -= increment / 2.0f;
                }
                if (ypos < 0) {
                    ypos += increment / 2.0f;
                }
                else {
                    ypos -= increment / 2.0f;
                }
                xpos += (increment * (rt::cRand() - 0.5));
                ypos += (increment * (rt::cRand() - 0.5));
                Ray ray;
                ray.origin = glm::vec3(0,0,0); //scene.camera.position;
                ray.direction = glm::normalize(glm::vec3(xpos, ypos, scene.camera.distance)); // TODO: multiply by cam's rot matrix
                int bounceCount = 0;
                float distance = 0.0f;
                buffer.pixels[(y * buffer.width) + x] += castRay(ray, scene, maxBounce, bounceCount, distance);
            }
        }
    }

    void cpuDepth(Scene scene, Buffer buffer, float& minDepth, float& maxDepth, bool antialias) {
        float increment = 1.0f / (float)buffer.width;
        float fw = (increment * (float)buffer.height) / 2;
        #pragma omp parallel for schedule(dynamic)
        for (int y = 0; y < buffer.height; y++) {
            for (int x = 0; x < buffer.width; x++) {
                float xpos = ((float)x * increment) - 0.5f;
                float ypos = -(((float)y * increment) - fw);
                if (xpos < 0) {
                    xpos += increment / 2.0f;
                }
                else {
                    xpos -= increment / 2.0f;
                }
                if (ypos < 0) {
                    ypos += increment / 2.0f;
                }
                else {
                    ypos -= increment / 2.0f;
                }
                if (antialias) {
                    xpos += (increment * (rt::cRand() - 0.5));
                    ypos += (increment * (rt::cRand() - 0.5));
                }
                Ray ray;
                ray.origin = glm::vec3(0,0,0); //scene.camera.position;
                ray.direction = glm::normalize(glm::vec3(xpos, ypos, scene.camera.distance)); // TODO: multiply by cam's rot matrix
                int bounceCount = 0;
                glm::vec3 albedo;
                glm::vec3 normal;
                float distance = 0.0f;
                probeRay(ray, scene, albedo, normal, distance);
                if (distance >= 0 && distance < minDepth) {
                    minDepth = distance;
                }
                if (distance > maxDepth) {
                    maxDepth = distance;
                }
                buffer.pixels[(y * buffer.width) + x] += glm::vec3(distance, distance, distance);
            }
        }
    }

    void cpuNormals(Scene scene, Buffer buffer, bool antialias) {
        float increment = 1.0f / (float)buffer.width;
        float fw = (increment * (float)buffer.height) / 2;
        #pragma omp parallel for schedule(dynamic)
        for (int y = 0; y < buffer.height; y++) {
            for (int x = 0; x < buffer.width; x++) {
                float xpos = ((float)x * increment) - 0.5f;
                float ypos = -(((float)y * increment) - fw);
                if (xpos < 0) {
                    xpos += increment / 2.0f;
                }
                else {
                    xpos -= increment / 2.0f;
                }
                if (ypos < 0) {
                    ypos += increment / 2.0f;
                }
                else {
                    ypos -= increment / 2.0f;
                }
                if (antialias) {
                    xpos += (increment * (rt::cRand() - 0.5));
                    ypos += (increment * (rt::cRand() - 0.5));
                }
                Ray ray;
                ray.origin = glm::vec3(0,0,0); //scene.camera.position;
                ray.direction = glm::normalize(glm::vec3(xpos, ypos, scene.camera.distance)); // TODO: multiply by cam's rot matrix
                int bounceCount = 0;
                glm::vec3 albedo;
                glm::vec3 normal;
                float distance = 0.0f;
                probeRay(ray, scene, albedo, normal, distance);
                buffer.pixels[(y * buffer.width) + x] += normal;
            }
        }
    }

    void cpuAlbedo(Scene scene, Buffer buffer, bool antialias) {
        float increment = 1.0f / (float)buffer.width;
        float fw = (increment * (float)buffer.height) / 2;
        #pragma omp parallel for schedule(dynamic)
        for (int y = 0; y < buffer.height; y++) {
            for (int x = 0; x < buffer.width; x++) {
                float xpos = ((float)x * increment) - 0.5f;
                float ypos = -(((float)y * increment) - fw);
                if (xpos < 0) {
                    xpos += increment / 2.0f;
                }
                else {
                    xpos -= increment / 2.0f;
                }
                if (ypos < 0) {
                    ypos += increment / 2.0f;
                }
                else {
                    ypos -= increment / 2.0f;
                }
                if (antialias) {
                    xpos += (increment * (rt::cRand() - 0.5));
                    ypos += (increment * (rt::cRand() - 0.5));
                }
                Ray ray;
                ray.origin = glm::vec3(0,0,0); //scene.camera.position;
                ray.direction = glm::normalize(glm::vec3(xpos, ypos, scene.camera.distance)); // TODO: multiply by cam's rot matrix
                int bounceCount = 0;
                glm::vec3 albedo;
                glm::vec3 normal;
                float distance = 0.0f;
                probeRay(ray, scene, albedo, normal, distance);
                buffer.pixels[(y * buffer.width) + x] += albedo;
            }
        }
    }

    void cpuProbe(Scene scene, Buffer albedos, Buffer normals, bool antialias) {
        float increment = 1.0f / (float)albedos.width;
        float fw = (increment * (float)albedos.height) / 2;
        #pragma omp parallel for schedule(dynamic)
        for (int y = 0; y < albedos.height; y++) {
            for (int x = 0; x < albedos.width; x++) {
                float xpos = ((float)x * increment) - 0.5f;
                float ypos = -(((float)y * increment) - fw);
                if (xpos < 0) {
                    xpos += increment / 2.0f;
                }
                else {
                    xpos -= increment / 2.0f;
                }
                if (ypos < 0) {
                    ypos += increment / 2.0f;
                }
                else {
                    ypos -= increment / 2.0f;
                }
                if (antialias) {
                    xpos += (increment * (rt::cRand() - 0.5));
                    ypos += (increment * (rt::cRand() - 0.5));
                }
                Ray ray;
                ray.origin = glm::vec3(0,0,0); //scene.camera.position;
                ray.direction = glm::normalize(glm::vec3(xpos, ypos, scene.camera.distance)); // TODO: multiply by cam's rot matrix
                int bounceCount = 0;
                glm::vec3 albedo;
                glm::vec3 normal;
                float distance = 0.0f;
                probeRay(ray, scene, albedo, normal, distance);
                albedos.pixels[(y * albedos.width) + x] += albedo;
                normals.pixels[(y * normals.width) + x] += normal;
            }
        }
    }
};