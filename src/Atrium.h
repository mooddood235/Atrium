#pragma once
#include <GLFW/glfw3.h>
#include <string>

struct AtriumData {
public:
    AtriumData(
        char* scenePath,
        char* envTexturePath,
        unsigned int depth,
        unsigned int samples,
        unsigned int width,
        unsigned int height,
        bool interactive,
        char* outPath
    ) {
        this->scenePath = scenePath;
        this->envTexturePath = envTexturePath;
        this->depth = depth;
        this->samples = samples;
        this->width = width;
        this->height = height;
        this->interactive = interactive;
        this->outPath = outPath;
        
    }
public:
    char* scenePath;
    char* envTexturePath;
    unsigned int depth;
    unsigned int samples;
    unsigned int width;
    unsigned int height;
    bool interactive;
    char* outPath;
};

GLFWwindow* InitGLFW(const AtriumData& atriumData);
void InitGLAD();
AtriumData ProcessCommandLine(int argc, char* argv[]);
void SaveImage(const char* outPath, const AtriumData& atriumData);
void APIENTRY GlDebugOutput(GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei length,
    const char* message,
    const void* userParam);