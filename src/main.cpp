#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/closest_point.hpp>
#include <algorithm>
#include <vector>
#include <chrono> 
#include <raytracing/raytracing.h>
#include <raytracing/material.h>
#include <raytracing/cpu.h>
#include <raytracing/geometry.h>
#include <raytracing/model.h>
#include <raytracing/postproc.h>
#include <raytracing/export.h>
#include <OpenImageDenoise/oidn.hpp>
using namespace std::chrono; 

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void printVec3(glm::vec3 vec) {
    printf("(%f; %f; %f)", vec.x, vec.y, vec.z);
}

int main(int, char**) {

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if __APPLE__
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Elysium v0.1", NULL, NULL);
    if (window == NULL) {
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    bool err = glewInit() != GLEW_OK;
    if (err) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
    ImGui::GetStyle().WindowRounding = 0;
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    io.Fonts->AddFontFromFileTTF("../misc/fonts/Roboto-Medium.ttf", 16.0f);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool showFileMenu = false;
    bool select = true;

    int dev = 0;

    int width = 800;
    int height = 800;
    char* framebuffer = (char*)malloc(width * height * 4);

    // <======================== Raytracing ========================>

    rt::Camera camera = rt::createCamera(glm::vec3(), glm::vec3(), 75);

    rt::Buffer buffer = rt::createBuffer(width, height);
    rt::Buffer albedoBuffer = rt::createBuffer(width, height);
    rt::Buffer normalBuffer = rt::createBuffer(width, height);

    rt::Material whiteWall = rtmat::lambertian(glm::vec3(1.0f, 1.0f, 1.0f));
    rt::Material redWall = rtmat::lambertian(glm::vec3(1.0f, 0.3f, 0.1f));
    rt::Material greenWall = rtmat::lambertian(glm::vec3(0.3f, 1.0f, 0.3f));
    rt::Material blueWall = rtmat::lambertian(glm::vec3(0.3f, 0.3f, 1.0f));
    rt::Material lightSource = rtmat::lightSource(glm::vec3(1.0f, 1.0f, 1.0f), 30.0f);

    blueWall.diffuseFactor = 0.0f;
    blueWall.translucentFactor = 0.95f;
    blueWall.specularFactor = 0.05f,
    blueWall.refractIndex = 1.65f;
    blueWall.translucentColor = glm::vec3(1.0f, 1.0f, 1.0f);
    blueWall.specularColor = glm::vec3(1.0f, 1.0f, 1.0f);

    rt::Object bottom_1 = rt::createTriangle(glm::vec3(-5, -5, 8), glm::vec3(5, -5, 8), glm::vec3(5, -5, 18), whiteWall);
    rt::Object bottom_2 = rt::createTriangle(glm::vec3(-5, -5, 8), glm::vec3(-5, -5, 18), glm::vec3(5, -5, 18), whiteWall);
    rt::Object top_1 = rt::createTriangle(glm::vec3(-5, 5, 8), glm::vec3(5, 5, 8), glm::vec3(5, 5, 18), whiteWall);
    rt::Object top_2 = rt::createTriangle(glm::vec3(-5, 5, 8), glm::vec3(-5, 5, 18), glm::vec3(5, 5, 18), whiteWall);
    rt::Object back_1 = rt::createTriangle(glm::vec3(-5, -5, 18), glm::vec3(5, -5, 18), glm::vec3(5, 5, 18), whiteWall);
    rt::Object back_2 = rt::createTriangle(glm::vec3(-5, -5, 18), glm::vec3(-5, 5, 18), glm::vec3(5, 5, 18), whiteWall);
    rt::Object left_1 = rt::createTriangle(glm::vec3(-5, -5, 8), glm::vec3(-5, -5, 18), glm::vec3(-5, 5, 18), redWall);
    rt::Object left_2 = rt::createTriangle(glm::vec3(-5, -5, 8), glm::vec3(-5, 5, 8), glm::vec3(-5, 5, 18), redWall);
    rt::Object right_1 = rt::createTriangle(glm::vec3(5, -5, 8), glm::vec3(5, -5, 18), glm::vec3(5, 5, 18), greenWall);
    rt::Object right_2 = rt::createTriangle(glm::vec3(5, -5, 8), glm::vec3(5, 5, 8), glm::vec3(5, 5, 18), greenWall);
    rt::Object light_1 = rt::createTriangle(glm::vec3(-1, 4.98f, 12), glm::vec3(1, 4.98f, 12), glm::vec3(1, 4.98f, 14), lightSource);
    rt::Object light_2 = rt::createTriangle(glm::vec3(-1, 4.98f, 12), glm::vec3(-1, 4.98f, 14), glm::vec3(1, 4.98f, 14), lightSource);

    std::vector<rt::Object> cube = rtgeo::cube(glm::vec3(-3.0f, -3.5f, 14), glm::vec3(), 2.5f, 2.5f, 2.5f, blueWall);

    std::vector<rt::Object> objModel = rtmodel::loadObj("teapot.obj", glm::vec3(0.0f, -5, 14), blueWall);

    std::vector<rt::Object> objects;
    objects.push_back(bottom_1);
    objects.push_back(bottom_2);
    objects.push_back(top_1);
    objects.push_back(top_2);
    objects.push_back(back_1);
    objects.push_back(back_2);
    objects.push_back(left_1);
    objects.push_back(left_2);
    objects.push_back(right_1);
    objects.push_back(right_2);
    objects.push_back(light_1);
    objects.push_back(light_2);

    rt::appendObjList(objects, cube);
    //rt::appendObjList(objects, objModel);

    rt::Scene scene = rt::compileScene(camera, objects);

    // Collect data for smoothing
    int metaSamples = 64;
    printf("Collecting scene info...\n");
    for (int i = 0; i < metaSamples; i++) {
        rt::cpuProbe(scene, albedoBuffer, normalBuffer, true);
    }
    rtpp::normalize(albedoBuffer, metaSamples);
    rtpp::normalize(normalBuffer, metaSamples);


    int iterations = 100;

    auto start = high_resolution_clock::now();

    for (int i = 0; i < iterations; i++) {
        if (i % 1 == 0) {
            printf("Progress: %f percent, Samples: %d\n", ((float)i / (float)iterations) * 100, i);
        }
        rt::cpuRender(scene, buffer, 7);
    }

    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);
    float seconds = (float)duration.count() / 1000000.0f;
    printf("Render time: %fs, FPS: %f", seconds, (float)iterations / seconds);

    rtpp::normalize(buffer, iterations);
    rtpp::intelDenoise(buffer, albedoBuffer, normalBuffer);
    rtpp::gamma(buffer, 2.2f);
    rtexp::toRGBA(buffer, framebuffer);
    rtexp::savePNG(buffer, "test_rt.png");


    // <======================== Raytracing ========================>



    // <======================== Save result in texture ========================>

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer);

    // <======================== Save result in texture ========================>
    

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //ImGui::ShowDemoWindow();

        ImGui::Image((void*)(intptr_t)image_texture, ImVec2(width, height));


        // Rendering #222f3e
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.13333f, 0.18431f, 0.24313f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}