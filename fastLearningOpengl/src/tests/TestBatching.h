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

class Batching : public Test
{
public:
    Batching();
    ~Batching() override;

    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;

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
};

} // namespace test