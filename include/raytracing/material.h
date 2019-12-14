#pragma once
#include <raytracing/raytracing.h>

namespace rtmat {
    inline rt::Material blank() {
        rt::Material mat;
        mat.ambiantColor = glm::vec3(0.0f, 0.0f, 0.0f);
        mat.diffuseColor = glm::vec3(0.0f, 0.0f, 0.0f);
        mat.specularColor = glm::vec3(0.0f, 0.0f, 0.0f);
        mat.emissiveColor = glm::vec3(0.0f, 0.0f, 0.0f);
        mat.translucentColor = glm::vec3(0.0f, 0.0f, 0.0f);
        mat.ambiantFactor = 0.0f;
        mat.diffuseFactor = 0.0f;
        mat.specularFactor = 0.0f;
        mat.shinyness = 0.0f;
        mat.translucency = 0.0f;
        mat.hasTexture = false;
        mat.isEmissive = false;
        mat.emissionBrightness = 0.0f;
        return mat;
    }

    inline rt::Material lambertian(glm::vec3 color) {
        rt::Material mat = blank();
        mat.diffuseColor = color;
        mat.diffuseFactor = 1.0f;
        return mat;
    }

    inline rt::Material lightSource(glm::vec3 color, float brightness) {
        rt::Material mat = blank();
        mat.emissiveColor = color;
        mat.emissionBrightness = brightness;
        mat.isEmissive = true;
        return mat;
    }
};