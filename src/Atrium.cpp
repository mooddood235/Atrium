#include <iostream>
#include <glad/glad.h>
#include <string>
#include <algorithm>

#include "../src_display/Quad.h"

#include "Atrium.h"
#include "Scene.h"
#include "Buffer.h"
#include "RenderCamera.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Film.h"
#include "BVH.h"

int main()
{
    // Inits
    GLFWwindow* window = InitGLFW();
    InitGLAD();

    glDebugMessageCallback(GlDebugOutput, nullptr);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    Atrium::RenderCamera::Init();
    
    // Objects
    Atrium::Scene cubeScene = Atrium::Scene("Models/Statue.gltf");
    std::cout << cubeScene.ToString() << std::endl;

    Atrium::BVH bvh = Atrium::BVH(cubeScene, Atrium::SplitMethod::SAH);

    Atrium::Camera* camera = cubeScene.cameras[0];
    Atrium::Film film(WINDOWWIDTH, WINDOWHEIGHT);
    Atrium::Texture environmentMap("EnvironmentMaps/blue_photo_studio_4k.hdr");
    
    // Display Objects
    Quad quad = Quad();
    Atrium::ShaderProgram displayShader("src_display/Shaders/display.vert", "src_display/Shaders/display.frag");

    // Objects
    float lastTime = 0.0f;
    unsigned int samplesTaken = 0;
    const unsigned int samplesPerTick = 1;
    const unsigned int depth = 10;

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        
        Atrium::RenderMode renderMode = Atrium::RenderMode::Append;
        if (camera->TransformFromInput(window, deltaTime)) {
            samplesTaken = 0;
            renderMode = Atrium::RenderMode::Write;
        }
        
        Atrium::RenderCamera::Render(film, *camera, bvh, environmentMap, samplesPerTick, depth, samplesTaken, renderMode);

        samplesTaken += samplesPerTick;

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, film.GetTextureID());
        displayShader.SetUint("samplesTaken", samplesTaken);
        displayShader.Use();
        quad.Draw();
        displayShader.UnUse();

        glFlush();
        //glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

GLFWwindow* InitGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(WINDOWWIDTH, WINDOWHEIGHT, "Atrium", NULL, NULL);

    if (!window) {
        std::cout << "ERROR: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    return window;
}
void InitGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "ERROR: Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
}
void APIENTRY GlDebugOutput(GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei length,
    const char* message,
    const void* userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}