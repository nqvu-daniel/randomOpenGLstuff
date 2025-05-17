#pragma once

#include "Test.h"
#include "VertexBuffer.h"
#include "ElementIndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include <memory>

namespace test{
    class TestTexture2D : public Test{
        public:
            TestTexture2D();
            ~TestTexture2D();

            void OnUpdate(float deltaTime) override;
            void OnRender() override;
            void OnImGuiRender() override;

        private:
            glm::vec3 m_TranslationA;
            glm::vec3 m_TranslationB;
            std::unique_ptr<Shader> m_Shader;
            std::unique_ptr<VertexArray> m_VAO;
            std::unique_ptr<VertexBuffer> m_VBO;
            std::unique_ptr<ElementIndexBuffer> m_EBO;
            std::unique_ptr<Texture> m_Texture;

            glm::mat4 m_Proj, m_View;
    };
};