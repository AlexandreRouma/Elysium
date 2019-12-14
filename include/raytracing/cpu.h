#pragma once
#include <raytracing/raytracing.h>

namespace rt {
    void cpuRender(Scene scene, Buffer buffer, int maxBounce);
    void cpuDepth(Scene scene, Buffer buffer, float& minDepth, float& maxDepth, bool antialias = false);
    void cpuNormals(Scene scene, Buffer buffer, bool antialias = false);
    void cpuAlbedo(Scene scene, Buffer buffer, bool antialias = false);
    void cpuProbe(Scene scene, Buffer albedos, Buffer normals, bool antialias);
};