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
      // m_view(glm::lookAt(glm::vec3(480.0f, 270.0f, 700.0f),  // Camera at (center_x, center_y, distance_z)
      //                    glm::vec3(480.0f, 270.0f, 0.0f),   // Looking at (center_x, center_y, 0)
      //                    glm::vec3(0.0f, 1.0f, 0.0f))),  // Up vector is positive Y
      // Initialize translation of the entire batch to origin
      m_translation(0.0f, 0.0f, 0.0f),
      // Model matrix starts as identity, will be updated by m_translation
      m_model(glm::mat4(1.0f)),
      // Initialize cube properties
      m_cubeCenter(480.0f, 400.0f, 0.0f), // Base center for all cubes
      m_cubeSize(300.0f),
      m_cubeColor{1.0f, 1.0f, 1.0f, 1.0f}, // Default white color
      m_cubeOffsets({
        glm::vec3( 0.0f, 0.0f, 0.0f),
        glm::vec3( 2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f, 2.0f, -2.5f),
        glm::vec3( 1.5f, 0.2f, -1.5f)
        // Add more offsets as needed, these are scaled by m_cubeSize for final positioning
      })
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
    std::vector<Vertex> batched_pos;
    std::vector<unsigned int> batched_indices;
    unsigned int current_offset = 0;

    // Create the 6 faces for a single cube template (centered at origin conceptually for CreateQuad)
    // The actual positioning will be handled by the model matrix in OnRender
    auto q_front  = CreateQuad(glm::vec3(0.0f), m_cubeSize, CubeFace::Front, 0, m_cubeColor);
    auto q_back   = CreateQuad(glm::vec3(0.0f), m_cubeSize, CubeFace::Back, 0, m_cubeColor);
    auto q_left   = CreateQuad(glm::vec3(0.0f), m_cubeSize, CubeFace::Left, 0, m_cubeColor);
    auto q_right  = CreateQuad(glm::vec3(0.0f), m_cubeSize, CubeFace::Right, 0, m_cubeColor);
    auto q_top    = CreateQuad(glm::vec3(0.0f), m_cubeSize, CubeFace::Top, 0, m_cubeColor);
    auto q_bottom = CreateQuad(glm::vec3(0.0f), m_cubeSize, CubeFace::Bottom, 0, m_cubeColor);

    std::vector<Vertex> single_cube_vertices;
    single_cube_vertices.insert(single_cube_vertices.end(), q_front.begin(), q_front.end());
    single_cube_vertices.insert(single_cube_vertices.end(), q_back.begin(), q_back.end());
    single_cube_vertices.insert(single_cube_vertices.end(), q_left.begin(), q_left.end());
    single_cube_vertices.insert(single_cube_vertices.end(), q_right.begin(), q_right.end());
    single_cube_vertices.insert(single_cube_vertices.end(), q_top.begin(), q_top.end());
    single_cube_vertices.insert(single_cube_vertices.end(), q_bottom.begin(), q_bottom.end());

    std::vector<unsigned int> single_cube_indices;
    for (size_t i = 0; i < 6; ++i) { // For each of the 6 faces
        single_cube_indices.push_back(i * 4 + 0);
        single_cube_indices.push_back(i * 4 + 1);
        single_cube_indices.push_back(i * 4 + 2);
        single_cube_indices.push_back(i * 4 + 2);
        single_cube_indices.push_back(i * 4 + 3);
        single_cube_indices.push_back(i * 4 + 0);
    }

    // For this approach, we only need to buffer one cube's geometry
    // The different positions will be handled by transforming and drawing multiple times in OnRender
    m_vertexBuffer->BufferSubData(single_cube_vertices.data(), unsigned(single_cube_vertices.size() * sizeof(Vertex)));
    m_indexBuffer = std::make_unique<ElementIndexBuffer>(single_cube_indices.data(), unsigned(single_cube_indices.size()));

}

void BatchingDynamic3D::OnRender()
{
    glCall(glEnable(GL_DEPTH_TEST)); // Enable depth testing for 3D
    glCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
    glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)); // Clear depth buffer as well

    m_texture0->Bind(0);
    m_texture1->Bind(1);

    // Set up the view matrix to orbit m_cubeCenter
    float orbitRadius = 800.0f; // Radius of the camera's orbit around m_cubeCenter
    float time = static_cast<float>(glfwGetTime());
    float camX = m_cubeCenter.x + static_cast<float>(sin(time) * orbitRadius);
    float camY = m_cubeCenter.y + 200.0f; // Position camera above the center Y
    float camZ = m_cubeCenter.z + static_cast<float>(cos(time) * orbitRadius);
    
    m_view = glm::lookAt(glm::vec3(camX, camY, camZ),   // Camera's new orbiting position
                         m_cubeCenter,                  // Look at the base center of the cubes
                         glm::vec3(0.0f, 1.0f, 0.0f));  // Up vector

    m_shader->Bind();
    m_shader->SetUniformMat4f("view", m_view);
    m_shader->SetUniformMat4f("projection", m_proj);
    m_shader->SetUniformVec1i("u_Textures", std::vector<int>{0, 1}); // Set texture IDs for the shader
    
    m_vao->Bind();
    if (m_indexBuffer) // Check if index buffer is valid before drawing
    {
        m_indexBuffer->Bind();

        for (const auto& offset : m_cubeOffsets)
        {
            glm::vec3 currentCubeActualCenter = m_cubeCenter + offset * m_cubeSize; // Scale offset by cube size
            float time = (float)glfwGetTime();
            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), time * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
            glm::mat4 translateToFinalPosition = glm::translate(glm::mat4(1.0f), currentCubeActualCenter);
            glm::mat4 modelMatrixForThisInstance = translateToFinalPosition * rotation; // Since template is at origin, no initial translateToOrigin needed if CreateQuad makes it so.

            // 4. Finally, apply the global m_translation (batch translation)
            m_model = glm::translate(glm::mat4(1.0f), m_translation) * modelMatrixForThisInstance;
            
            m_shader->SetUniformMat4f("model", m_model);
            m_renderer.Draw(*m_vao, *m_indexBuffer, *m_shader);
        }
    }
}

void BatchingDynamic3D::OnImGuiRender()
{
    ImGui::SliderFloat3("Cube Base Center", &m_cubeCenter.x, 0.0f, 960.0f); 
    ImGui::SliderFloat("Cube Size", &m_cubeSize, 10.0f, 500.0f);
    ImGui::ColorEdit4("Cube Color", m_cubeColor.data());
    ImGui::SliderFloat3("Batch Translation", &m_translation.x, -500.0f, 500.0f); 
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

};