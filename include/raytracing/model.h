#pragma once
#include <raytracing/raytracing.h>
#include <string>
#include <iostream>
#include <fstream>

namespace rtmodel {
    std::vector<rt::Object> loadObj(std::string path, glm::vec3 position, rt::Material material);
};