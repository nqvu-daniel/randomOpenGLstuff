#include "TestBatchingDynamic3D.h"

#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "imgui/imgui.h"
#include "Texture.h"
#include "glm/gtc/matrix_transform.hpp" // For glm::lookAt and glm::perspective
#include "GLFW/glfw3.h" // For glfwGetTime
#include <tuple>
#include <vector>
#include <algorithm> 

namespace test
{


BatchingDynamic3D::BatchingDynamic3D()
    : // Initialize with a perspective projection
      m_proj(glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 1500.0f)), // Adjusted far plane
      // Set up a camera view looking at the center of the 0-960, 0-540 area
      m_view(glm::lookAt(glm::vec3(480.0f, 270.0f, 700.0f),  // Camera at (center_x, center_y, distance_z)
                         glm::vec3(480.0f, 270.0f, 0.0f),   // Looking at (center_x, center_y, 0)
                         glm::vec3(0.0f, 1.0f, 0.0f))),  // Up vector is positive Y
      // Initialize translation of the entire batch to origin
      m_translation(0.0f, 0.0f, 0.0f),
      // Model matrix starts as identity, will be updated by m_translation
      m_model(glm::mat4(1.0f)),
      // Initialize cube properties
      m_cubeCenter(480.0f, 400.0f, 0.0f), // Centered in the view
      m_cubeSize(300.0f),
      m_cubeColor{1.0f, 1.0f, 1.0f, 1.0f} // Default white color
{

    m_vao = std::make_unique<VertexArray>();
    m_vao->Bind();

    constexpr auto magic_count = 1000u; // magic number for the allocated number of vertices!

    m_vertexBuffer = std::make_unique<VertexBuffer>(nullptr, sizeof(BatchingDynamic3D::Vertex) * magic_count);
    m_vertexBuffer->Bind();


    VertexBufferLayout layout;
    layout.Push<float>(std::tuple_size_v<decltype(Vertex::position)>); // 3 floats for position (x, y, z)
    layout.Push<float>(std::tuple_size_v<decltype(Vertex::texCoords)>); // 2 floats for texture coordinates (u, v)
    layout.Push<float>(std::tuple_size_v<decltype(Vertex::color)>); // 4 floats for color (r, g, b, a)
    layout.Push<float>(1); // texture id

    m_vao->AddBuffer(*m_vertexBuffer, layout); 

    
    m_shader = std::make_unique<Shader>("res/Shaders/BatchColor3D.shader"); 
    m_shader->Bind();
    m_texture0 = std::make_unique<Texture>("res/Textures/cute.png");
    m_texture0->Bind(0); // Bind texture to slot 0
    // m_shader->SetUniform1i("u_Texture", 0); // Tell shader to use texture slot 0 OLD UNUSED
    m_texture1 = std::make_unique<Texture>("res/Textures/ChernoLogo.png"); 
    m_texture1->Bind(1); 


}

BatchingDynamic3D::~BatchingDynamic3D()
{
    // Unique_ptrs will handle deletion
    glCall(glDisable(GL_DEPTH_TEST)); // Disable depth test when this test is exited
}

void BatchingDynamic3D::OnUpdate([[maybe_unused]] float deltaTime)
{
    // Create cube using quads
    auto q_front  = CreateQuad(m_cubeCenter, m_cubeSize, CubeFace::Front, 0, m_cubeColor);
    auto q_back   = CreateQuad(m_cubeCenter, m_cubeSize, CubeFace::Back, 0, m_cubeColor);
    auto q_left   = CreateQuad(m_cubeCenter, m_cubeSize, CubeFace::Left, 0, m_cubeColor);
    auto q_right  = CreateQuad(m_cubeCenter, m_cubeSize, CubeFace::Right, 0, m_cubeColor);
    auto q_top    = CreateQuad(m_cubeCenter, m_cubeSize, CubeFace::Top, 0, m_cubeColor);
    auto q_bottom = CreateQuad(m_cubeCenter, m_cubeSize, CubeFace::Bottom, 0, m_cubeColor);
    
    // maybe ill update this to be dynamic later

    std::vector<Vertex> batched_pos;
    batched_pos.insert(batched_pos.end(), q_front.begin(), q_front.end());
    batched_pos.insert(batched_pos.end(), q_back.begin(), q_back.end());
    batched_pos.insert(batched_pos.end(), q_left.begin(), q_left.end());
    batched_pos.insert(batched_pos.end(), q_right.begin(), q_right.end());
    batched_pos.insert(batched_pos.end(), q_top.begin(), q_top.end());
    batched_pos.insert(batched_pos.end(), q_bottom.begin(), q_bottom.end());

    std::vector<unsigned int> batched_indices;
    unsigned int offset = 0;
    for (size_t i = 0; i < 6; ++i) { // For each of the 6 faces
        batched_indices.push_back(offset + 0);
        batched_indices.push_back(offset + 1);
        batched_indices.push_back(offset + 2);
        batched_indices.push_back(offset + 2);
        batched_indices.push_back(offset + 3);
        batched_indices.push_back(offset + 0);
        offset += 4; // Each quad has 4 vertices
    }

    m_indexBuffer = std::make_unique<ElementIndexBuffer>(batched_indices.data(), unsigned(batched_indices.size()));
    m_vertexBuffer->BufferSubData(batched_pos.data(), unsigned(batched_pos.size() * sizeof(Vertex)));
}

void BatchingDynamic3D::OnRender()
{
    glCall(glEnable(GL_DEPTH_TEST)); // Enable depth testing for 3D
    glCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
    glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)); // Clear depth buffer as well

    m_texture0->Bind(0);
    m_texture1->Bind(1);
    
    m_shader->Bind();

    // Corrected model matrix calculation:
    // 1. Create a matrix to translate the cube to the origin (relative to its center)
    glm::mat4 translateToOrigin = glm::translate(glm::mat4(1.0f), -m_cubeCenter);
    // 2. Create the rotation matrix (rotating around the now origin)
    // Using the original rotation: around X-axis, animated by time
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() * glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // 3. Create a matrix to translate the cube back to its original center
    glm::mat4 translateBack = glm::translate(glm::mat4(1.0f), m_cubeCenter);

    // Combine these to get a matrix that rotates the cube around its own center
    glm::mat4 localPivotRotation = translateBack * rotation * translateToOrigin;

    // 4. Finally, apply the global m_translation (batch translation)
    m_model = glm::translate(glm::mat4(1.0f), m_translation) * localPivotRotation;
    
    m_shader->SetUniformMat4f("model", m_model);
    m_shader->SetUniformMat4f("view", m_view);
    m_shader->SetUniformMat4f("projection", m_proj);
    m_shader->SetUniformVec1i("u_Textures", std::vector<int>{0, 1}); // Set texture IDs for the shader
    
    m_vao->Bind();
    if (m_indexBuffer) // Check if index buffer is valid before drawing
    {
        m_indexBuffer->Bind();
        // Shader is already bound
        m_renderer.Draw(*m_vao, *m_indexBuffer, *m_shader);
    }
}

void BatchingDynamic3D::OnImGuiRender()
{
    ImGui::SliderFloat3("Cube Center", &m_cubeCenter.x, 0.0f, 960.0f); 
    ImGui::ColorEdit4("Cube Color", m_cubeColor.data());
    ImGui::SliderFloat3("Batch Translation", &m_translation.x, -500.0f, 500.0f); 
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

};