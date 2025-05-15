#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <string>
#include "Renderer.h"
#define DEBUG

#include "VertexBuffer.h"
#include "ElementIndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
// #undef DEBUG



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
    GLFWwindow* window = glfwCreateWindow(800, 600, "red triangle", NULL, NULL);
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
    glCall(glViewport(0, 0, 800, 600));
    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
         -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
    };

    unsigned int indices[] = {
        0,1,2,
        0,2,3

    };


    glCall(glEnable(GL_BLEND));
    glCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    VertexArray VAO;

    VertexBuffer vb(vertices, sizeof(vertices));
    VertexBufferLayout layout;
    layout.Push<float>(3); // 3 floats per vertex
    layout.Push<float>(2); // 2 floats per vertex for texture coordinates
    VAO.AddBuffer(vb, layout);
    // already bounded in the intialization of VertexBuffer
    // multiple VBO => manual binding of choosing => handled by the vertex array anyways lol



    ElementIndexBuffer ebo(indices, sizeof(indices)); // similar to VBO

    

    Shader shader("../res/Shaders/Basic.shader");
    shader.Bind();

    Texture texture("../res/Textures/cute.png");
    texture.Bind(); // defaulted to slot 0

    shader.SetUniform1i("u_Texture", 0); // slot 0 for the texture
    
    float tempRed = 0.0f;
    float increment = 0.05f;


    
    vb.Unbind();
    ebo.Unbind();
    VAO.Unbind();
    shader.Unbind();

    Renderer renderer;

    // render loopss
    while(!glfwWindowShouldClose(window)){
        processInput(window);

        renderer.Clear();

        shader.Bind();
        renderer.Draw(VAO, ebo, shader);

        // update shader uniform
        double  timeValue = glfwGetTime();
        float greenValue = static_cast<float>(sin(timeValue) / 2.0 + 0.5);

        shader.SetUniform4f("ourColor", tempRed, greenValue, 0.0f, 1.0f);



        tempRed += increment;
        if (tempRed > 1.0f){
            increment = -0.05f;
        } else if (tempRed < 0.0f){
            increment = 0.05f;
        }


        // event checks n calls; buffer swap
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //glCall(glDeleteProgram(shader)); <- destructor called
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