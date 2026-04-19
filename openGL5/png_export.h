#pragma once

#ifdef __APPLE__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <GLUT/glut.h>
#pragma GCC diagnostic pop
#else
#include <GL/glut.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

inline bool savePNG(int w, int h) {
    std::vector<unsigned char> pixels(w * h * 4);
    glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    // Flip vertically (OpenGL bottom-up vs PNG top-down)
    int rowBytes = w * 4;
    std::vector<unsigned char> row(rowBytes);
    for (int y = 0; y < h / 2; ++y) {
        unsigned char* top = &pixels[y * rowBytes];
        unsigned char* bot = &pixels[(h - 1 - y) * rowBytes];
        std::memcpy(row.data(), top, rowBytes);
        std::memcpy(top, bot, rowBytes);
        std::memcpy(bot, row.data(), rowBytes);
    }

    char filename[128];
    std::time_t now = std::time(nullptr);
    std::strftime(filename, sizeof(filename),
                  "drawing-%Y%m%d-%H%M%S.png", std::localtime(&now));

    int ok = stbi_write_png(filename, w, h, 4, pixels.data(), rowBytes);
    if (ok) {
        std::printf("[DrawTool] Saved PNG: %s\n", filename);
    } else {
        std::printf("[DrawTool] Failed to save PNG\n");
    }
    return ok != 0;
}
