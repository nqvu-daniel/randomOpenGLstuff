#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
// #include "glm/gtc/type_ptr.hpp"

struct ShaderProgramSource{
    std::string VertexSource;
    std::string FragmentSource;
};


class Shader
{
private:
    /* data */
    std::string m_FilePath;
    unsigned int m_RendererID;
    // caching system for uniforms
    std::unordered_map<std::string, int> m_UniformLocationCache;

public:
    Shader(const std::string& filepath);
    ~Shader();

    void Bind() const;
    void Unbind() const;


    // Set uniforms
    void SetUniform1i(const std::string& name, int value);
    void SetUniform1f(const std::string& name, float value);
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
    void SetUniformVec1i(const std::string &name, const std::vector<int> &vector);

private:
    int GetUniformLocation(const std::string& name);
    
    ShaderProgramSource ParseShader(const std::string& filepath);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

};

