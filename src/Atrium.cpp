#include <iostream>
#include <glad/glad.h>
#include <string>
#include <algorithm>
#include <stb/stb_image_write.h>

#include "../src_display/Quad.h"

#include "Atrium.h"
#include "Scene/Scene.h"
#include "RenderCamera.h"
#include "ShaderProgram.h"
#include "Texture/Film.h"
#include "Texture/EnvironmentMap.h"

int main(int argc, char* argv[])
{
    AtriumData atriumData = ProcessCommandLine(argc, argv);

    // Inits
    GLFWwindow* window = InitGLFW(atriumData);
    InitGLAD();

    glDebugMessageCallback(GlDebugOutput, nullptr);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    Atrium::RenderCamera::Init();
    
    // Objects
    Atrium::Scene scene = Atrium::Scene(atriumData.scenePath);
    std::cout << scene.ToString() << std::endl;

    if (scene.cameras.empty()) {
        std::cout << "LOAD GLTF ERROR: You must load a scene with a camera in it." << std::endl;
        exit(-1);
    }

    Atrium::Camera* camera = scene.cameras[0];
    Atrium::Film film(atriumData.width, atriumData.height);
    Atrium::EnvironmentMap environmentMap(atriumData.envTexturePath);
    
    // Display Objects
    Quad quad = Quad();
    Atrium::ShaderProgram displayShader("src_display/Shaders/display.vert", "src_display/Shaders/display.frag");

    // Objects
    float lastTime = 0.0f;
    const unsigned int maxSamples = atriumData.samples;
    unsigned int samplesTaken = 0;
    const unsigned int samplesPerTick = 1;
    const unsigned int depth = atriumData.depth;

    // Bind scene
    Atrium::RenderCamera::BindScene(scene);

    // Render loop
    while (!glfwWindowShouldClose(window) && (atriumData.interactive || samplesTaken < maxSamples)) {
        glClear(GL_COLOR_BUFFER_BIT);

        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        
        Atrium::RenderMode renderMode = Atrium::RenderMode::Append;
        if (atriumData.interactive && camera->TransformFromInput(window, deltaTime)) {
            samplesTaken = 0;
            renderMode = Atrium::RenderMode::Write;
        }
        if (!atriumData.interactive && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            std::cout << samplesTaken << "/" << maxSamples << std::endl;
        }
        if (samplesTaken < maxSamples) {
            Atrium::RenderCamera::Render(film, *camera, environmentMap, samplesPerTick, depth, samplesTaken, renderMode);
            samplesTaken += samplesPerTick;
        }
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
    Atrium::RenderCamera::UnBindScene(scene);
    
    if (!atriumData.interactive && atriumData.outPath) SaveImage(atriumData.outPath, atriumData);
    
    glfwTerminate();
    return 0;
}

GLFWwindow* InitGLFW(const AtriumData& atriumData) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(atriumData.width, atriumData.height, "Atrium", NULL, NULL);

    if (!window) {
        std::cout << "ERROR: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    if (atriumData.interactive) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    return window;
}
void InitGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "ERROR: Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
}
AtriumData ProcessCommandLine(int argc, char* argv[]){
    auto IsNum = [](char* s) -> bool {
        for (unsigned int i = 0; i < strlen(s); i++)
            if (!isdigit(s[i])) return false;
        return true;
    };
    for (unsigned int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "help") == 0 || strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "-h") == 0) {
            std::cout << "\n./Atrium [ScenePath] [EnvMapPath] [MaxDepth] [Samples] [Width] [Height] | Optional Flags" << std::endl;
            std::cout << "Optional flags:\n  -i for interactive mode \n  -o followed by image out path (only png supported). Only works if -i is not set." << std::endl;
            std::cout << "Press 's' to show progress in non-interactive mode." << std::endl;
            exit(1);
        }
    }
    if (argc < 7) {
        std::cout << "ERROR: ./Atrium [ScenePath] [EnvMapPath] [MaxDepth] [Samples] [Width] [Height] | Optional Flags" << std::endl;
        exit(-1);
    }
    if (!(IsNum(argv[3]) && IsNum(argv[4]) && IsNum(argv[5]) && IsNum(argv[6]))) {
        std::cout << "ERROR: [MaxDepth], [Samples], [Width], and [Height] must be unsigned ints." << std::endl;
        exit(-1);
    }
    bool interactive = false;
    char* outPath = nullptr;

    for (unsigned int i = 5; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 && (strcmp(argv[i - 1], "-o") != 0)) interactive = true;
        if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 >= argc) {
                std::cout << "ERROR: Expected out path following -o." << std::endl;
                exit(-1);
            }
            outPath = argv[i + 1];
        }
    }
    return AtriumData(argv[1], argv[2], stoi(argv[3]), stoi(argv[4]), stoi(argv[5]), stoi(argv[6]), interactive, outPath);
}
void SaveImage(const char* outPath, const AtriumData& atriumData) {
    unsigned char* pixels = new unsigned char[atriumData.width * atriumData.height * 3];
    glReadPixels(0, 0, atriumData.width, atriumData.height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    stbi_flip_vertically_on_write(true);
    stbi_write_png(outPath, atriumData.width, atriumData.height, 3, pixels, 3 * atriumData.width);
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
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204 || id == 131154) return;

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