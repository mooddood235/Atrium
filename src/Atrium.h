#pragma once
#include <GLFW/glfw3.h>

#define WINDOWWIDTH 1920
#define WINDOWHEIGHT 1080


GLFWwindow* InitGLFW();
void InitGLAD();
void APIENTRY GlDebugOutput(GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei length,
    const char* message,
    const void* userParam);