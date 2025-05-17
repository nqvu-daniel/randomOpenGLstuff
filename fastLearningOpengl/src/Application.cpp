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



    float vertices[] = { // pos[x,y...]
			-50.0f, -50.0f, 0.0f, 0.0f, 0.0f, // 0
			 50.0f, -50.0f, 0.0f, 1.0f, 0.0f, // 1
			 50.0f,  50.0f, 0.0f, 1.0f, 1.0f, // 2
			-50.0f,  50.0f, 0.0f, 0.0f, 1.0f, // 3
		};

		unsigned int indices[] = { // trig[v1,v2,v3...]
			0, 1, 2,
			2, 3, 0
		};


    glCall(glEnable(GL_BLEND));
    glCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    VertexArray VAO;

    VertexBuffer vb(vertices, sizeof(vertices));
    VertexBufferLayout layout;
    layout.Push<float>(3); // 3 floats per vertex for pos
    layout.Push<float>(2); // 2 floats per vertex for texture coordinates
    VAO.AddBuffer(vb, layout);


    ElementIndexBuffer ebo(indices, sizeof(indices));

    glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));


    Shader shader("../res/Shaders/Basic.shader");
    shader.Bind();

    Texture texture("../res/Textures/cute.png");
    texture.Bind(); // defaulted to slot 0

    shader.SetUniform1i("u_Texture", 0); // slot 0 for the texture



    //float tempRed = 0.0f;
    //float increment = 0.05f;
    glm::vec3 translationA(200, 200, 0);
    glm::vec3 translationB(400, 200, 0);

    vb.Unbind();
    ebo.Unbind();
    VAO.Unbind();
    shader.Unbind();

    Renderer renderer;




    // Imgui initialization
    // Setup Dear ImGui context (literally just copied this off docs)
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

    // render loops
    while(!glfwWindowShouldClose(window)){
        processInput(window);

        renderer.Clear();
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow(); // Show demo window! :)

        shader.Bind();

        {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
        glm::mat4 mvp = proj * view * model;
        shader.Bind();
        shader.SetUniformMat4f("u_MVP", mvp);
        renderer.Draw(VAO, ebo, shader);
        }

            // note that on more robust framework, there should be a caching system for shaders instead of continuously binding it like this
        {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
        glm::mat4 mvp = proj * view * model;
        shader.Bind();
        shader.SetUniformMat4f("u_MVP", mvp);
        renderer.Draw(VAO, ebo, shader);
        }

        // NOT THE SOLUTION FOR EVERYTHING E.G. TILING! this is better fit for "batching" (shoving everything into a single draw call - VAO)


        //tempRed += increment;
        //if (tempRed > 1.0f){
        //    increment = -0.05f;
        //} else if (tempRed < 0.0f){
        //    increment = 0.05f;
        //}
        // update shader uniform
        //double  timeValue = glfwGetTime();
        //float greenValue = static_cast<float>(sin(timeValue) / 2.0 + 0.5);
        //shader.SetUniform4f("ourColor", tempRed, greenValue, 0.0f, 1.0f);


        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {

            ImGui::SliderFloat3("TranslationA", &translationA.x, 0.0f,960.0f);
            ImGui::SliderFloat3("TranslationB", &translationB.x, 0.0f,960.0f);            
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        }



        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // event checks n calls; buffer swap
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
