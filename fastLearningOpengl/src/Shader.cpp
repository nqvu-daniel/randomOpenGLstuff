#include "Shader.h"
#include "Renderer.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>




Shader::Shader(const std::string& filepath)
    : m_FilePath(filepath), m_RendererID(0)
{
    ShaderProgramSource source = ParseShader(filepath);
    std::cout << "Vertex" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "Fragment" << std::endl;
    std::cout << source.FragmentSource << std::endl;

    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    glCall(glDeleteProgram(m_RendererID));
}


ShaderProgramSource Shader::ParseShader(const std::string& filepath){
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

unsigned int Shader::CompileShader(unsigned int type, const std::string& source){
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

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader){
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


void Shader::Bind() const
{
    glCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
    glCall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    glCall(glUniform1i(GetUniformLocation(name), value));
}
void Shader::SetUniform1f(const std::string& name, float value)
{
    glCall(glUniform1f(GetUniformLocation(name), value));
}

// if math library is row major, then GL_TRUE, if column major, then GL_FALSE
void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    glCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) return m_UniformLocationCache[name];
        // if uniform doesn't exist, create it

        glCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
        if (location == -1) {
            std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
        }
            m_UniformLocationCache[name] = location;
        return location;
}