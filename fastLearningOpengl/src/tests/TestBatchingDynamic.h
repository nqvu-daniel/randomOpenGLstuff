#pragma once

#include "Test.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <memory>
// #include <vector> // Not strictly needed in the header if only used in .cpp

// Include necessary headers directly
#include "VertexArray.h"
#include "ElementIndexBuffer.h"
#include "VertexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Renderer.h" // Renderer is used as a member

namespace test
{

class BatchingDynamic : public Test
{
public:
    BatchingDynamic();
    ~BatchingDynamic() override;

    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;

        
    struct Vertex
    {
        std::array<float, 3> position {0.0f, 0.0f, 0.0f}; // X, Y, Z
        std::array<float, 2> texCoords{0.0f, 0.0f}; // U, V
        std::array<float, 4> color {0.0f, 0.0f, 0.0f, 0.0f}; // R, G, B, A
        float textureID; // Texture ID
    };

    // im too lazy to change this
    static std::array<Vertex, 4> CreateQuad(float x, float y, float size, float textureID)
    {   
        size = 50.0f; // TODO: remove this
        // legend : x, y, z, u, v, r, g, b, a, ID
        Vertex v0 = {{x, y, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, textureID};
        Vertex v1 = {{x + size, y, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, textureID};
        Vertex v2 = {{x + size, y + size, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, textureID};
        Vertex v3 = {{x, y + size, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, textureID};
        return {v0, v1, v2, v3};
    };


private:
    std::unique_ptr<VertexArray> m_vao;
    std::unique_ptr<ElementIndexBuffer> m_indexBuffer;
    std::unique_ptr<VertexBuffer> m_vertexBuffer;
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<Texture> m_texture0;
    std::unique_ptr<Texture> m_texture1;

    glm::mat4 m_proj;
    glm::mat4 m_view;
    glm::vec3 m_translation;

    Renderer m_renderer; // Renderer member
    float m_quad0position[2] = {100.0f, 50.0f};
    float m_quad1position[2] = {350.0f, 50.0f};
};

} // namespace test