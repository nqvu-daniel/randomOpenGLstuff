#pragma once

#include "Test.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <memory>
#include "VertexArray.h"
#include "ElementIndexBuffer.h"
#include "VertexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"


enum class CubeFace {
    Front, Back, Left, Right, Top, Bottom
};

namespace test
{

class BatchingDynamic3D : public Test
{
public:
    BatchingDynamic3D();
    ~BatchingDynamic3D() override;

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

    // Updated CreateQuad to generate faces of a cube
    static std::array<Vertex, 4> CreateQuad(const glm::vec3& center, float size, CubeFace face, float textureID, const std::array<float, 4>& color = {1.0f, 1.0f, 1.0f, 1.0f})
    {   
        std::array<Vertex, 4> vertices;
        float hs = size / 2.0f;

        // Standard texture coordinates (BottomLeft, BottomRight, TopRight, TopLeft)
        std::array<float, 2> tc0 = {0.0f, 0.0f}; 
        std::array<float, 2> tc1 = {1.0f, 0.0f};
        std::array<float, 2> tc2 = {1.0f, 1.0f};
        std::array<float, 2> tc3 = {0.0f, 1.0f};

        switch (face) {
            case CubeFace::Front: // +Z normal, CCW: BL, BR, TR, TL
                vertices[0] = {{center.x - hs, center.y - hs, center.z + hs}, tc0, color, textureID};
                vertices[1] = {{center.x + hs, center.y - hs, center.z + hs}, tc1, color, textureID};
                vertices[2] = {{center.x + hs, center.y + hs, center.z + hs}, tc2, color, textureID};
                vertices[3] = {{center.x - hs, center.y + hs, center.z + hs}, tc3, color, textureID};
                break;
            case CubeFace::Back: // -Z normal, CCW from outside: BL, TL, TR, BR ((-hs,-hs,-hs), (-hs,+hs,-hs), (+hs,+hs,-hs), (+hs,-hs,-hs))
                vertices[0] = {{center.x - hs, center.y - hs, center.z - hs}, tc0, color, textureID}; // BL
                vertices[1] = {{center.x - hs, center.y + hs, center.z - hs}, tc3, color, textureID}; // TL
                vertices[2] = {{center.x + hs, center.y + hs, center.z - hs}, tc2, color, textureID}; // TR
                vertices[3] = {{center.x + hs, center.y - hs, center.z - hs}, tc1, color, textureID}; // BR
                break;
            case CubeFace::Left: // -X normal, CCW from outside: BL, BR, TR, TL ((-hs,-hs,-hs), (-hs,-hs,+hs), (-hs,+hs,+hs), (-hs,+hs,-hs))
                vertices[0] = {{center.x - hs, center.y - hs, center.z - hs}, tc0, color, textureID}; // Bottom-Back
                vertices[1] = {{center.x - hs, center.y - hs, center.z + hs}, tc1, color, textureID}; // Bottom-Front
                vertices[2] = {{center.x - hs, center.y + hs, center.z + hs}, tc2, color, textureID}; // Top-Front
                vertices[3] = {{center.x - hs, center.y + hs, center.z - hs}, tc3, color, textureID}; // Top-Back
                break;
            case CubeFace::Right: // +X normal, CCW from outside: BL, TL, TR, BR ((+hs,-hs,+hs), (+hs,+hs,+hs), (+hs,+hs,-hs), (+hs,-hs,-hs))
                vertices[0] = {{center.x + hs, center.y - hs, center.z + hs}, tc0, color, textureID}; // Bottom-Front
                vertices[1] = {{center.x + hs, center.y + hs, center.z + hs}, tc3, color, textureID}; // Top-Front
                vertices[2] = {{center.x + hs, center.y + hs, center.z - hs}, tc2, color, textureID}; // Top-Back
                vertices[3] = {{center.x + hs, center.y - hs, center.z - hs}, tc1, color, textureID}; // Bottom-Back
                break;
            case CubeFace::Top: // +Y normal, CCW from outside: BL, BR, TR, TL ((-hs,+hs,+hs), (+hs,+hs,+hs), (+hs,+hs,-hs), (-hs,+hs,-hs))
                vertices[0] = {{center.x - hs, center.y + hs, center.z + hs}, tc0, color, textureID}; // Front-Left
                vertices[1] = {{center.x + hs, center.y + hs, center.z + hs}, tc1, color, textureID}; // Front-Right
                vertices[2] = {{center.x + hs, center.y + hs, center.z - hs}, tc2, color, textureID}; // Back-Right
                vertices[3] = {{center.x - hs, center.y + hs, center.z - hs}, tc3, color, textureID}; // Back-Left
                break;
            case CubeFace::Bottom: // -Y normal, CCW from outside: BL, TL, TR, BR ((-hs,-hs,-hs), (-hs,-hs,+hs), (+hs,-hs,+hs), (+hs,-hs,-hs))
                vertices[0] = {{center.x - hs, center.y - hs, center.z - hs}, tc0, color, textureID}; // Back-Left
                vertices[1] = {{center.x - hs, center.y - hs, center.z + hs}, tc3, color, textureID}; // Front-Left
                vertices[2] = {{center.x + hs, center.y - hs, center.z + hs}, tc2, color, textureID}; // Front-Right
                vertices[3] = {{center.x + hs, center.y - hs, center.z - hs}, tc1, color, textureID}; // Back-Right
                break;
        }
        return vertices;
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
    glm::mat4 m_model; // Model matrix for transformations

    Renderer m_renderer; // Renderer member
    
    // New members for cube properties
    glm::vec3 m_cubeCenter;
    float m_cubeSize;
    std::array<float, 4> m_cubeColor;
};

} // namespace test