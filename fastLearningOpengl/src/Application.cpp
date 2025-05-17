#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "Renderer.h"
#define DEBUG

#include "VertexBuffer.h"
#include "ElementIndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "tests/TestClearColor.h"
#include "tests/TestTexture2D.h"
#include "tests/TestBatching.h"

// #undef DEBUG

// NOTE: you should build this using cmake instead of using vscode c++ build task system

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

int main(){

    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing (MSAA)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    // create window
    GLFWwindow* window = glfwCreateWindow(960, 540, "NOT a red triangle", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // make the window's context current
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1); // enable vsync
    #ifdef __APPLE__
        gladLoadGL();
    #else
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    #endif

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glCall(glViewport(0, 0, 960, 540));
    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);

    // tools version
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "glfw version: " << glfwGetVersionString() << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "GPU: " << glGetString(GL_VENDOR) << std::endl;


    glCall(glEnable(GL_BLEND));
    glCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));


    Renderer renderer;


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init(nullptr);


    std::cout << "Info: ImGui version: " << IMGUI_VERSION;
    #ifdef IMGUI_HAS_DOCK
    	std::cout << " +docking";
    #endif
    #ifdef IMGUI_HAS_VIEWPORT
    	std::cout << " +viewport";
    #endif
    	std::cout << std::endl << std::endl;



    test::Test* currentTest = nullptr;
    test::TestMenu* testMenu = new test::TestMenu(currentTest);
    currentTest = testMenu;

    testMenu->RegisterTest<test::TestClearColor>("Clear Color");
    testMenu->RegisterTest<test::TestTexture2D>("2D Texture");
    testMenu->RegisterTest<test::Batching>("Batching");

    // render loops
    while(!glfwWindowShouldClose(window)){
        processInput(window);
        
        glCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));

        renderer.Clear();


        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // ImGui::ShowDemoWindow(); 

        if (currentTest){
            currentTest->OnUpdate(0.0f);
            currentTest->OnRender();
            ImGui::Begin("Tests");
            if (currentTest != testMenu){
                if (ImGui::Button("<-")){
                    delete currentTest;
                    currentTest = testMenu;
                }
            }
            currentTest->OnImGuiRender();
            ImGui::End();
        } else {
            ImGui::Begin("Tests");
            ImGui::Text("No test selected");
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glCall(glViewport(0,0,width,height));
}

void processInput(GLFWwindow *window){
    if(glfwGetKey(window,GLFW_KEY_ESCAPE)==GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
}