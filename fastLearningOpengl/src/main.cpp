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
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
         -0.5f, 0.5f, 0.0f
    };

    unsigned int indices[] = {
        0,1,2,
        0,2,3

    };


    VertexArray VAO;

    VertexBuffer vb(vertices, sizeof(vertices));
    VertexBufferLayout layout;
    layout.Push<float>(3); // 3 floats per vertex
    VAO.AddBuffer(vb, layout);
    // already bounded in the intialization of VertexBuffer
    // multiple VBO => manual binding of choosing => handled by the vertex array anyways lol



    ElementIndexBuffer ebo(indices, sizeof(indices)); // similar to VBO

    
    //shader
 //   ShaderProgramSource source = ParseShader("../res/Shaders/Basic.shader");
 //   std::cout << "Vertex" << std::endl;
 //   std::cout << source.VertexSource << std::endl;
 //   std::cout << "Fragment" << std::endl;
 //   std::cout << source.FragmentSource << std::endl;
//
 //    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
 //   glCall(glUseProgram(shader));

    Shader shader("../res/Shaders/Basic.shader");
    shader.Bind();

    
    float tempRed = 0.0f;
    float increment = 0.05f;


    
    vb.Unbind();
    ebo.Unbind();
    VAO.Unbind();
    shader.Unbind();

    // render loopss
    while(!glfwWindowShouldClose(window)){
        processInput(window);

        // rendering command
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // <-- state setting function
        glCall(glClear(GL_COLOR_BUFFER_BIT)); // <-- state using function

        // bind and do stuff
        shader.Bind();
        // bind vertex array
        VAO.Bind();
        ebo.Bind();


        // 6 being the number of INDICES; nullptr cuz we bounded the EBO earlier, so no need to specify the vertices again
        glCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
        // would give nothing if type is GL_INT => ALL index buffer must be of type GL_UNSIGNED_INT

        // update shader uniform
        double  timeValue = glfwGetTime();
        float greenValue = static_cast<float>(sin(timeValue) / 2.0 + 0.5);


        //new shader
        shader.SetUniform4f("ourColor", tempRed, greenValue, 0.0f, 1.0f);
        // legacy code
        //glCall(int vertexColorLocation = glGetUniformLocation(shader, "ourColor"));
        //glCall(glUniform4f(vertexColorLocation, tempRed, greenValue, 0.0f, 1.0f));

        // draw call draws the entire element  -> cant change color cuz glUniform4f is state setting function

        // update red value
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