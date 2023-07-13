#pragma once
#include <GLFW/glfw3.h>
#include <string>

#define WINDOWWIDTH 1920
#define WINDOWHEIGHT 1080

struct AtriumData {
public:
    AtriumData(char* scenePath, char* envTexturePath, unsigned int samples, unsigned int depth) {
        this->scenePath = scenePath;
        this->envTexturePath = envTexturePath;
        this->samples = samples;
        this->depth = depth;
    }
public:
    char* scenePath;
    char* envTexturePath;
    unsigned int samples;
    unsigned int depth;
};

GLFWwindow* InitGLFW();
void InitGLAD();
AtriumData ProcessCommandLine(int argc, char* argv[]);
void APIENTRY GlDebugOutput(GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei length,
    const char* message,
    const void* userParam);