#pragma once
#include <raytracing/raytracing.h>

namespace rtgeo {
    inline std::vector<rt::Object> quad(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, rt::Material material) {
        std::vector<rt::Object> objList;
        objList.push_back(rt::createTriangle(a, b, c, material));
        objList.push_back(rt::createTriangle(a, c, d, material));
        return objList;
    }

    inline std::vector<rt::Object> cube(glm::vec3 center, glm::vec3 rotation, float width, float height, float depth, rt::Material material) {
        std::vector<rt::Object> objList;
        // TODO: Add rotation
        glm::vec3 ba = glm::vec3(-width / 2.0f, -height / 2.0f, depth / 2.0f) + center;
        glm::vec3 bb = glm::vec3(width / 2.0f, -height / 2.0f, depth / 2.0f) + center;
        glm::vec3 bc = glm::vec3(width / 2.0f, -height / 2.0f, -depth / 2.0f) + center;
        glm::vec3 bd = glm::vec3(-width / 2.0f, -height / 2.0f, -depth / 2.0f) + center;
        glm::vec3 ta = glm::vec3(-width / 2.0f, height / 2.0f, depth / 2.0f) + center;
        glm::vec3 tb = glm::vec3(width / 2.0f, height / 2.0f, depth / 2.0f) + center;
        glm::vec3 tc = glm::vec3(width / 2.0f, height / 2.0f, -depth / 2.0f) + center;
        glm::vec3 td = glm::vec3(-width / 2.0f, height / 2.0f, -depth / 2.0f) + center;
        rt::appendObjList(objList, quad(ba, bb, bc, bd, material)); // Bottom
        rt::appendObjList(objList, quad(ta, tb, tc, td, material)); // Top
        rt::appendObjList(objList, quad(td, ta, ba, bd, material)); // Left
        rt::appendObjList(objList, quad(tc, tb, bb, bc, material)); // Right
        rt::appendObjList(objList, quad(ta, tb, bb, ba, material)); // Back
        rt::appendObjList(objList, quad(td, tc, bc, bd, material)); // Front
        return objList;
    }
};