#include "TestBatchingDynamic.h"

#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "imgui/imgui.h"
#include "Texture.h"

#include <tuple>
#include <vector>
#include <algorithm> // For std::transform if needed, or manual loop for clarity

namespace test
{


BatchingDynamic::BatchingDynamic()
    : m_proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
      m_view(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
      m_translation(200.0f, 150.0f, 0.0f)
{

    m_vao = std::make_unique<VertexArray>();
    m_vao->Bind();

    constexpr auto magic_count = 1000u; // magic number for the allocated number of vertices!

    m_vertexBuffer = std::make_unique<VertexBuffer>(nullptr, sizeof(BatchingDynamic::Vertex) * magic_count);
    m_vertexBuffer->Bind();


    VertexBufferLayout layout;
    layout.Push<float>(std::tuple_size_v<decltype(Vertex::position)>); // 3 floats for position (x, y, z)
    layout.Push<float>(std::tuple_size_v<decltype(Vertex::texCoords)>); // 2 floats for texture coordinates (u, v)
    layout.Push<float>(std::tuple_size_v<decltype(Vertex::color)>); // 4 floats for color (r, g, b, a)
    layout.Push<float>(1); // texture id

    m_vao->AddBuffer(*m_vertexBuffer, layout); 

    
    m_shader = std::make_unique<Shader>("res/Shaders/BatchColor.shader"); 
    m_shader->Bind();
    m_texture0 = std::make_unique<Texture>("res/Textures/cute.png");
    m_texture0->Bind(0); // Bind texture to slot 0
    //m_shader->SetUniform1i("u_Texture", 0); // Tell shader to use texture slot 0
    m_texture1 = std::make_unique<Texture>("res/Textures/ChernoLogo.png"); 
    m_texture1->Bind(1); 


}

BatchingDynamic::~BatchingDynamic()
{
    // Unique_ptrs will handle deletion
}

void BatchingDynamic::OnUpdate([[maybe_unused]] float deltaTime)
{
    // No per-frame updates needed for this simple example beyond ImGui interaction
        // bam i used it 
    auto q0 = CreateQuad(m_quad0position[0], m_quad0position[1], 50.0f, 0.0f);
    auto q1 = CreateQuad(m_quad1position[0], m_quad1position[1], 50.0f, 1.0f);
    // maybe ill update this to be dynamic later

    std::vector<Vertex> batched_pos(q0.size() + q1.size()); // Reserve space for the combined vertices
    memcpy(batched_pos.data()/*            */, q0.data(), q0.size() * sizeof(Vertex));
    memcpy(batched_pos.data() + q0.size(), q1.data(), q1.size() * sizeof(Vertex));

    std::vector<unsigned int>quad0_indices = {0, 1, 2, 2, 3, 0};
    std::vector<unsigned int>quad1_indices = {0, 1, 2, 2, 3, 0};


    std::vector<unsigned int> batched_indices(quad0_indices);
    std::transform(quad1_indices.begin(), quad1_indices.end(), std::back_inserter(batched_indices),
                   [&](const auto &val){return val + q0.size();});
                   //[offset = quad1_indices.size()](unsigned int index) { return index + offset; });

    m_indexBuffer = std::make_unique<ElementIndexBuffer>(batched_indices.data(), unsigned(batched_indices.size()));
    m_vertexBuffer->BufferSubData(batched_pos.data(), unsigned(batched_pos.size() * sizeof(Vertex)));
}

void BatchingDynamic::OnRender()
{
    glCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
    glCall(glClear(GL_COLOR_BUFFER_BIT));

    m_texture0->Bind(0);
    m_texture1->Bind(1);
    m_shader->Bind();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), m_translation);
    glm::mat4 mvp = m_proj * m_view * model;
    m_shader->SetUniformMat4f("u_MVP", mvp);
    m_shader->SetUniformVec1i("u_Textures", std::vector<int>{0, 1}); // Set texture IDs for the shader
    m_vao->Bind();
    m_indexBuffer->Bind();
    m_shader->Bind();

    m_renderer.Draw(*m_vao, *m_indexBuffer, *m_shader);
}

void BatchingDynamic::OnImGuiRender()
{
    ImGui::SliderFloat3("Batch Translation", &m_translation.x, 0.0f, 960.0f - 50.0f); // Adjust max range as needed
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::SliderFloat2("Quad 1 'C'", m_quad0position, 0.0f, 960.0f);
	ImGui::SliderFloat2("Quad 2 'H'", m_quad1position, 0.0f, 960.0f);
}

} // namespace test