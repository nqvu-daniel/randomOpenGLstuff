#include "TestTexture2D.h"
#include "Renderer.h"
#include "imgui/imgui.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace test{

    TestTexture2D::TestTexture2D() : m_TranslationA(200, 200, 0), m_TranslationB(400, 200, 0), 
    m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
    m_View( glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)))
    {

        float vertices[] = { // pos[x,y...]
			-50.0f, -50.0f, 0.0f, 0.0f, 0.0f, // 0
			 50.0f, -50.0f, 0.0f, 1.0f, 0.0f, // 1
			 50.0f,  50.0f, 0.0f, 1.0f, 1.0f, // 2
			-50.0f,  50.0f, 0.0f, 0.0f, 1.0f, // 3
		};

		unsigned int indices[] = { // trig[v1,v2,v3...]
			0, 1, 2,
			2, 3, 0
		};

    glCall(glEnable(GL_BLEND));
    glCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    
    m_VAO = std::make_unique<VertexArray>();

    m_EBO = std::make_unique<ElementIndexBuffer>(indices, sizeof(indices));// / sizeof(unsigned int));

    m_VBO = std::make_unique<VertexBuffer>(vertices, sizeof(vertices));
    VertexBufferLayout layout;
    layout.Push<float>(3); // 3 floats per vertex for pos
    layout.Push<float>(2); // 2 floats per vertex for texture coordinates

    m_VAO->AddBuffer(*m_VBO, layout);

    m_Shader = std::make_unique<Shader>("res/Shaders/BasicLegacy.shader");
    m_Shader->Bind();
    m_Texture = std::make_unique<Texture>("res/Textures/cute.png");

    m_Shader->SetUniform1i("ourTexture", 0); // slot 0 for the texture

    }
    


    TestTexture2D::~TestTexture2D()
    {
    }
    


    void TestTexture2D::OnUpdate(float deltaTime)
    {
    }
    


    void TestTexture2D::OnRender()
    {
        glCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
        glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        Renderer renderer;

        m_Texture->Bind(); // bind the texture to slot 0
        {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
        glm::mat4 mvp = m_Proj * m_View * model;
        m_Shader->Bind();
        m_Shader->SetUniformMat4f("u_MVP", mvp);
        renderer.Draw(*m_VAO, *m_EBO, *m_Shader);
        }

            // note that on more robust framework, there should be a caching system for shaders instead of continuously binding it like this
        {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
        glm::mat4 mvp = m_Proj * m_View * model;
        m_Shader->Bind();
        m_Shader->SetUniformMat4f("u_MVP", mvp);
        renderer.Draw(*m_VAO, *m_EBO, *m_Shader);
        }
    }
    

    void TestTexture2D::OnImGuiRender()
    {
            ImGuiIO& io = ImGui::GetIO();
            ImGui::SliderFloat3("TranslationA", &m_TranslationA.x, 0.0f,960.0f);
            ImGui::SliderFloat3("TranslationB", &m_TranslationB.x, 0.0f,960.0f);            
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    }


    

}