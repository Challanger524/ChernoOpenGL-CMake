#pragma once

#include "Test.hpp"
#include "Utility.hpp"

#include "Renderer.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include "Texture.hpp"

#include <GL/glew.h>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <type_traits>

namespace test
{

class TestTexture2D : public Test
{
	std::unique_ptr<VertexArray> m_vao;
	std::unique_ptr<IndexBuffer> m_indexBuffer;
	std::unique_ptr<VertexBuffer> m_vertexBuffer;
	std::unique_ptr<Shader> m_shader;
	std::unique_ptr<Texture> m_texture;

	glm::mat4 m_proj = glm::ortho(0.0f, 960.0f, 0.0f, 720.0f, -1.0f, 1.0f);
	glm::mat4 m_view = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0));
	glm::vec3 m_translationA = glm::vec3(200, 200, 0);
	glm::vec3 m_translationB = glm::vec3(400, 200, 0);

	Renderer m_renderer;

public:
	~TestTexture2D() {}
	TestTexture2D()
	{
		float positions[] = { // pos[x,y...]
			-50.0f, -50.0f, 0.0f, 0.0f, // 0
			 50.0f, -50.0f, 1.0f, 0.0f, // 1
			 50.0f,  50.0f, 1.0f, 1.0f, // 2
			-50.0f,  50.0f, 0.0f, 1.0f, // 3
		};

		unsigned int indices[] = { // trig[v1,v2,v3...]
			0, 1, 2,
			2, 3, 0
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		m_vao = std::make_unique<VertexArray>();

		static_assert(sizeof(float) == sizeof(std::remove_extent_t<decltype(positions)>));
		m_vertexBuffer = std::make_unique<VertexBuffer>(positions, unsigned(4 * 4 * sizeof(std::remove_extent_t<decltype(positions)>)));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		m_vao->AddBuffer(*m_vertexBuffer, layout);

		m_indexBuffer = std::make_unique<IndexBuffer>(indices, 6);

		m_texture = std::make_unique<Texture>("res/textures/ChernoLogo.png");

		m_shader = std::make_unique<Shader>("res/Shaders/Basic.shader");
		m_shader->Bind();
		m_shader->SetUniform1i("u_Texture", 0);
	}

	void OnUpdate([[maybe_unused]] float deltaTime = 0.0f) override {}
	void OnRender() override
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		m_texture->Bind(0);

		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_translationA);
			glm::mat4 mvp = m_proj * m_view * model;

			m_shader->Bind();
			m_shader->SetUniformMat4f("u_MVP", mvp);

			m_renderer.Draw(*m_vao, *m_indexBuffer, *m_shader);
		}

		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_translationB);
			glm::mat4 mvp = m_proj * m_view * model;

			m_shader->Bind();
			m_shader->SetUniformMat4f("u_MVP", mvp);

			m_renderer.Draw(*m_vao, *m_indexBuffer, *m_shader);
		}
	}
	void OnImGuiRender() override
	{
		ImGui::SliderFloat2("TranslationA", &m_translationA.x, 0.0f, 960.0f);
		ImGui::SliderFloat2("TranslationB", &m_translationB.x, 0.0f, 960.0f);
	}
};

}
