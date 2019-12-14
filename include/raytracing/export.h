#pragma once
#include <raytracing/raytracing.h>
#include <stdint.h>
#include <string>
#include <iostream>
#include <fstream>
#include <type_traits>

#define RTEXP_STBI_SAVE

#ifdef RTEXP_STBI_SAVE
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#endif

namespace rtexp {
    inline void toRGBA(rt::Buffer buf, char* rgba) {
        for (int y = 0; y < buf.height; y++) {
            for (int x = 0; x < buf.width; x++) {
                rgba[(y * buf.width * 4) + (x * 4) + 0] = (buf.pixels[(y * buf.width) + x].r * 255);
                rgba[(y * buf.width * 4) + (x * 4) + 1] = (buf.pixels[(y * buf.width) + x].g * 255);
                rgba[(y * buf.width * 4) + (x * 4) + 2] = (buf.pixels[(y * buf.width) + x].b * 255);
                rgba[(y * buf.width * 4) + (x * 4) + 3] = 255;
            }
        }
    }

#ifdef RTEXP_STBI_SAVE
    inline void saveBMP(rt::Buffer buf, std::string path) {
        char* pixels = (char*)malloc(buf.width * buf.height * 4);
        toRGBA(buf, pixels);
        stbi_write_bmp(path.c_str(), buf.width, buf.height, 4, pixels);
        free(pixels);
    }

    inline void savePNG(rt::Buffer buf, std::string path) {
        char* pixels = (char*)malloc(buf.width * buf.height * 4);
        toRGBA(buf, pixels);
        stbi_write_png(path.c_str(), buf.width, buf.height, 4, pixels, buf.width * 4);
        free(pixels);
    }
#endif
}