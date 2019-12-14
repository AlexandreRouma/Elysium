#pragma once
#include <raytracing/raytracing.h>

#define POSTPROC_INTEL_DENOISE

#ifdef POSTPROC_INTEL_DENOISE
#include <OpenImageDenoise/oidn.hpp>
#endif

namespace rtpp {
    inline void normalize(rt::Buffer buf, float max) {
        int pcount = buf.width * buf.height;
        for (int i = 0; i < pcount; i++) {
            buf.pixels[i] = glm::clamp(buf.pixels[i], 0.0f, (float)max) / (float)max;
        }
    }

    inline void gamma(rt::Buffer buf, float gamma) {
        int pcount = buf.width * buf.height;
        for (int i = 0; i < pcount; i++) {
            buf.pixels[i] = glm::pow(buf.pixels[i], glm::vec3(1.0f / gamma, 1.0f / gamma, 1.0f / gamma));
        }
    }

#ifdef POSTPROC_INTEL_DENOISE

    inline void intelDenoise(rt::Buffer buf) {
        rt::Buffer temp = rt::createBuffer(buf.width, buf.height);
        oidn::DeviceRef device = oidn::newDevice();
        device.commit();
        oidn::FilterRef filter = device.newFilter("RT"); // generic ray tracing filter
        filter.setImage("color",  buf.pixels,  oidn::Format::Float3, buf.width, buf.height);
        filter.setImage("output", temp.pixels, oidn::Format::Float3, buf.width, buf.height);
        filter.set("hdr", false); // image is HDR
        filter.commit();
        filter.execute();
        const char* errorMessage;
        if (device.getError(errorMessage) != oidn::Error::None) {
            std::cout << "Error: " << errorMessage << std::endl;
        }
        rt::copyBuffer(temp, buf);
    }

    inline void intelDenoise(rt::Buffer buf, rt::Buffer albedo) {
        rt::Buffer temp = rt::createBuffer(buf.width, buf.height);
        oidn::DeviceRef device = oidn::newDevice();
        device.commit();
        oidn::FilterRef filter = device.newFilter("RT");
        filter.setImage("color",  buf.pixels,  oidn::Format::Float3, buf.width, buf.height);
        filter.setImage("albedo", albedo.pixels, oidn::Format::Float3, buf.width, buf.height);
        filter.setImage("output", temp.pixels, oidn::Format::Float3, buf.width, buf.height);
        filter.set("hdr", false);
        filter.commit();
        filter.execute();
        const char* errorMessage;
        if (device.getError(errorMessage) != oidn::Error::None) {
            std::cout << "Error: " << errorMessage << std::endl;
        }
        rt::copyBuffer(temp, buf);
    }

    inline void intelDenoise(rt::Buffer buf, rt::Buffer albedo, rt::Buffer normals) {
        rt::Buffer temp = rt::createBuffer(buf.width, buf.height);
        oidn::DeviceRef device = oidn::newDevice();
        device.commit();
        oidn::FilterRef filter = device.newFilter("RT");
        filter.setImage("color",  buf.pixels,  oidn::Format::Float3, buf.width, buf.height);
        filter.setImage("albedo", albedo.pixels, oidn::Format::Float3, buf.width, buf.height);
        filter.setImage("normal", normals.pixels, oidn::Format::Float3, buf.width, buf.height);
        filter.setImage("output", temp.pixels, oidn::Format::Float3, buf.width, buf.height);
        filter.set("hdr", false);
        filter.commit();
        filter.execute();
        const char* errorMessage;
        if (device.getError(errorMessage) != oidn::Error::None) {
            std::cout << "Error: " << errorMessage << std::endl;
        }
        rt::copyBuffer(temp, buf);
    }

#endif
}
