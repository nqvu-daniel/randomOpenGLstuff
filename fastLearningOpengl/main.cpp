#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>

#define DEBUG

#ifdef DEBUG
    #define glCall(x) glClearError(); x; assert(glLogCall())
#else
    #define glCall(x) x
#endif
// error checking code

static void glClearError(){
    while(glGetError()!= GL_NO_ERROR);
}

static bool glLogCall(){
    while(GLenum error = glGetError()){
        std::cerr << "\n[OpenGL Error] (" << error << ")\n"
                  << "----------------------------------------\n";
        return false;
    }
    return true;
}

struct ShaderProgramSource{
    std::string VertexSource;
    std::string FragmentSource;
};
static ShaderProgramSource ParseShader(const std::string& filepath){
    std::fstream stream(filepath);
    std::string line;
    
    enum class ShaderType{
        NONE = -1, VERTEX = 0, FRAGMENT = 1};
    ShaderType type = ShaderType::NONE;

    std::stringstream ss[2];

    while (getline(stream, line)){
        if (line.find("#shader") != std::string::npos){
            if (line.find("vertex") != std::string::npos){
                type = ShaderType::VERTEX;
            } else if (line.find("fragment") != std::string::npos){
                type = ShaderType::FRAGMENT;
            }
        } else {
            if(type != ShaderType::NONE){
                ss[(int)type] << line << '\n';};
        }
    };
    return {ss[0].str(), ss[1].str()};
};


static unsigned int CompileShader(unsigned int type, const std::string& source){
    glCall(unsigned int id = glCreateShader(type));
    const char* src = source.c_str();
    glCall(glShaderSource(id, 1, &src, nullptr));
    glCall(glCompileShader(id));

    int result;
    glCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    std::cout << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader compile status: " << result << std::endl;


    if(result == GL_FALSE){
        int length;
        glCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)alloca(length * sizeof(char));
        glCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        glCall(glDeleteShader(id));
        return 0;
    }
    return id;
};

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader){
    // taking in the actual source code of these shaders as strings.

    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    glCall(glAttachShader(program, vs));
    glCall(glAttachShader(program, fs));
    glCall(glLinkProgram(program));
    glCall(glValidateProgram(program));
    glCall(glDeleteShader(vs));
    glCall(glDeleteShader(fs));

    return program;

};

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
    glfwMakeContextCurrent(window);

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

    unsigned int VAO;
    glCall(glGenVertexArrays(1, &VAO));
    glCall(glBindVertexArray(VAO));

    unsigned int EBO;
    glCall(glGenBuffers(1, &EBO));
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    unsigned int VBO;
    glCall(glGenBuffers(1, &VBO));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    glCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0));
    glCall(glEnableVertexAttribArray(0));

    //shader
    ShaderProgramSource source = ParseShader("res/Shaders/Basic.shader");
    std::cout << "Vertex" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "Fragment" << std::endl;
    std::cout << source.FragmentSource << std::endl;

     unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glCall(glUseProgram(shader));
    
    float tempRed = 0.0f;
    float increment = 0.05f;
    // render loops
    while(!glfwWindowShouldClose(window)){
        processInput(window);

        // rendering command
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // <-- state setting function
        glCall(glClear(GL_COLOR_BUFFER_BIT)); // <-- state using function


        // 6 being the number of INDICES; nullptr cuz we bounded the EBO earlier, so no need to specify the vertices again
        glCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
        // would give nothing if type is GL_INT => ALL index buffer must be of type GL_UNSIGNED_INT

        // update shader uniform
        double  timeValue = glfwGetTime();
        float greenValue = static_cast<float>(sin(timeValue) / 2.0 + 0.5);
        glCall(int vertexColorLocation = glGetUniformLocation(shader, "ourColor"));
        glCall(glUniform4f(vertexColorLocation, tempRed, greenValue, 0.0f, 1.0f));

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

    glCall(glDeleteProgram(shader));
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