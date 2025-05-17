#include "TestCamera.h"

#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "imgui/imgui.h"
#include "Texture.h"
#include "glm/gtc/matrix_transform.hpp" // For glm::lookAt and glm::perspective
#include "GLFW/glfw3.h" // For glfwGetTime and key codes
#include <tuple>
#include <vector>
#include <algorithm> 
#include <cstdlib> // Added for rand()
#include <string>  // Added for std::to_string, though often included transitively

namespace test
{

// Initial camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  250.0f; // Adjusted speed
const float SENSITIVITY =  0.1f; // Ill implement this later
const float ZOOM        =  45.0f;

TestCameraSuite::TestCameraSuite()
    : m_proj(glm::perspective(glm::radians(ZOOM), 960.0f / 540.0f, 0.1f, 1500.0f)),
      m_translation(0.0f, 0.0f, 0.0f),
      m_model(glm::mat4(1.0f)),
      m_cubeCenter(480.0f, 400.0f, 0.0f),
      m_cubeSize(50.0f), // Smaller default cube size
      m_cubeColor{1.0f, 1.0f, 1.0f, 1.0f},
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
      }),
      m_cameraPos(glm::vec3(480.0f, 270.0f, 700.0f)), // Initial position
      m_cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)),
      m_cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)),
      m_yaw(YAW),
      m_pitch(PITCH)
{
    m_vao = std::make_unique<VertexArray>();
    m_vao->Bind();

    constexpr auto magic_count = 1000u; // magic number for the allocated number of vertices!

    m_vertexBuffer = std::make_unique<VertexBuffer>(nullptr, sizeof(TestCameraSuite::Vertex) * magic_count);
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
    m_texture1 = std::make_unique<Texture>("res/Textures/ChernoLogo.png"); 
    m_texture1->Bind(1); 

    // Initialize individual rotation offsets with random values
    m_individualCubeRotationOffsets.resize(m_cubeOffsets.size());
    for (size_t i = 0; i < m_individualCubeRotationOffsets.size(); ++i) {
        m_individualCubeRotationOffsets[i] = glm::vec3(
            static_cast<float>(rand() % 360 - 180), // Random angle between -180 and 180
            static_cast<float>(rand() % 360 - 180),
            static_cast<float>(rand() % 360 - 180)
        );
    }

    UpdateCameraVectors(); // Initialize camera vectors
}

TestCameraSuite::~TestCameraSuite()
{
    // Unique_ptrs will handle deletion
    glCall(glDisable(GL_DEPTH_TEST)); // Disable depth test when this test is exited
}

void TestCameraSuite::ProcessKeyboard(int key, float deltaTime)
{
    float velocity = SPEED * deltaTime;
    if (key == GLFW_KEY_W)
        m_cameraPos += m_cameraFront * velocity;
    if (key == GLFW_KEY_S)
        m_cameraPos -= m_cameraFront * velocity;
    if (key == GLFW_KEY_A)
        m_cameraPos -= glm::normalize(glm::cross(m_cameraFront, m_cameraUp)) * velocity;
    if (key == GLFW_KEY_D)
        m_cameraPos += glm::normalize(glm::cross(m_cameraFront, m_cameraUp)) * velocity;
    if (key == GLFW_KEY_SPACE) // Example: Move up
        m_cameraPos += m_cameraUp * velocity;
    if (key == GLFW_KEY_LEFT_SHIFT) // Example: Move down
        m_cameraPos -= m_cameraUp * velocity;
}

void TestCameraSuite::UpdateCameraVectors()
{
    // Recalculate the Front vector from the Camera's (updated) Euler Angles
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_cameraFront = glm::normalize(front);
}

void TestCameraSuite::OnUpdate([[maybe_unused]] float deltaTime)
{
    std::vector<Vertex> batched_pos;
    std::vector<unsigned int> batched_indices;

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

void TestCameraSuite::OnRender()
{
    glCall(glEnable(GL_DEPTH_TEST)); // Enable depth testing for 3D
    glCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
    glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)); // Clear depth buffer as well

    m_texture0->Bind(0);
    m_texture1->Bind(1);

    m_view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp); // Camera position and orientation

    m_shader->Bind();
    m_shader->SetUniformMat4f("view", m_view);
    m_shader->SetUniformMat4f("projection", m_proj);
    m_shader->SetUniformVec1i("u_Textures", std::vector<int>{0, 1}); // Set texture IDs for the shader
    
    m_vao->Bind();
    if (m_indexBuffer) // Check if index buffer is valid before drawing
    {
        m_indexBuffer->Bind();

        for (size_t i = 0; i < m_cubeOffsets.size(); ++i)
        {
            const auto& offset = m_cubeOffsets[i];
            const auto& rotationOffset = m_individualCubeRotationOffsets[i]; // Get individual rotation

            glm::vec3 currentCubeActualCenter = m_cubeCenter + offset * m_cubeSize; // Scale offset by cube size
            float time = (float)glfwGetTime();
            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), time * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
            rotation = glm::rotate(rotation, glm::radians(rotationOffset.x), glm::vec3(1.0f, 0.0f, 0.0f));
            rotation = glm::rotate(rotation, glm::radians(rotationOffset.y), glm::vec3(0.0f, 1.0f, 0.0f));
            rotation = glm::rotate(rotation, glm::radians(rotationOffset.z), glm::vec3(0.0f, 0.0f, 1.0f));
            glm::mat4 translateToFinalPosition = glm::translate(glm::mat4(1.0f), currentCubeActualCenter);
            glm::mat4 modelMatrixForThisInstance = translateToFinalPosition * rotation; // Since template is at origin, no initial translateToOrigin needed if CreateQuad makes it so

            // 4. Finally, apply the global m_translation (batch translation)
            m_model = glm::translate(glm::mat4(1.0f), m_translation) * modelMatrixForThisInstance;
            
            m_shader->SetUniformMat4f("model", m_model);
            m_renderer.Draw(*m_vao, *m_indexBuffer, *m_shader);
        }
    }
}

void TestCameraSuite::OnImGuiRender()
{
    ImGui::SliderFloat3("Cube Base Center", &m_cubeCenter.x, 0.0f, 960.0f); 
    ImGui::SliderFloat("Cube Size", &m_cubeSize, 10.0f, 500.0f);
    ImGui::ColorEdit4("Cube Color", m_cubeColor.data());
    ImGui::SliderFloat3("Batch Translation", &m_translation.x, -500.0f, 500.0f); 

    // Add ImGui controls for individual cube rotations
    if (ImGui::TreeNode("Individual Cube Rotations"))
    {
        for (size_t i = 0; i < m_individualCubeRotationOffsets.size(); ++i)
        {
            ImGui::PushID(static_cast<int>(i));
            std::string label = "Cube " + std::to_string(i) + " Rotation";
            ImGui::SliderFloat3(label.c_str(), &m_individualCubeRotationOffsets[i].x, -180.0f, 180.0f);
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    // Add ImGui controls for camera parameters (for debugging purposes)
    ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", m_cameraPos.x, m_cameraPos.y, m_cameraPos.z);
    ImGui::Text("Camera Front: (%.2f, %.2f, %.2f)", m_cameraFront.x, m_cameraFront.y, m_cameraFront.z);
    ImGui::Text("Yaw: %.2f, Pitch: %.2f", m_yaw, m_pitch);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

};