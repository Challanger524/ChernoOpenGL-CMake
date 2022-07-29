#pragma once

#include "Test.hpp"
#include "Utility.hpp"

#include "IndexBuffer.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

#include <GL/glew.h>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <array>
#include <vector>
#include <type_traits>

namespace test
{

struct Vertex {
	std::array<float, 2> position{ 0.0f, 0.0f };         // xy
	std::array<float, 4> color{ 0.0f, 0.0f,0.0f, 0.0f }; // rgba
	std::array<float, 2> texcoord{ 0.0f, 0.0f };         // xy
	float texId{ 0.f };                                  // <id>
};

class BatchingTexturesDynamic : public Test
{
	GLenum m_vertexArray;
	std::unique_ptr<IndexBuffer> m_indexBuffer;
	GLenum m_arrayBuffer;
	std::unique_ptr<Shader> m_shader;

	glm::mat4 m_proj = glm::ortho(0.0f, 960.0f, 0.0f, 720.0f, -1.0f, 1.0f);	   // screen scale
	glm::mat4 m_view = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0)); // camera
	glm::vec3 m_translation = glm::vec3(400, 200, 0);

	Texture m_chernoTex{ "res/textures/ChernoLogo.png" };
	Texture m_hazelTex{ "res/textures/HazelLogo.png" };

	float m_quad0Position[2] = { 100.0f, 100.0f };
	float m_quad1Position[2] = { 300.0f, 100.0f };

public:
	~BatchingTexturesDynamic() {
		GLCall(glDeleteBuffers(1, &m_arrayBuffer));
		GLCall(glDeleteVertexArrays(1, &m_vertexArray));
	}
	BatchingTexturesDynamic()
	{
		GLCall(glGenVertexArrays(1, &m_vertexArray));
		GLCall(glBindVertexArray(m_vertexArray));

		constexpr auto magic_count = 1000;
		GLCall(glGenBuffers(1, &m_arrayBuffer));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_arrayBuffer));
		GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * magic_count, nullptr, GL_DYNAMIC_DRAW));

		GLCall(glEnableVertexArrayAttrib(m_vertexArray, 0u));
		GLCall(glVertexAttribPointer(0u, std::tuple_size_v<decltype(Vertex::position)>, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void *>(offsetof(Vertex, position))));

		GLCall(glEnableVertexArrayAttrib(m_vertexArray, 1u));
		GLCall(glVertexAttribPointer(1u, std::tuple_size_v<decltype(Vertex::color)>, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void *>(offsetof(Vertex, color))));

		GLCall(glEnableVertexArrayAttrib(m_vertexArray, 2u));
		GLCall(glVertexAttribPointer(2u, std::tuple_size_v<decltype(Vertex::texcoord)>, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void *>(offsetof(Vertex, texcoord))));

		GLCall(glEnableVertexArrayAttrib(m_vertexArray, 3u));
		GLCall(glVertexAttribPointer(3u, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void *>(offsetof(Vertex, texId))));

		m_shader = std::make_unique<Shader>("res/Shaders/Batch-Textures.shader");
		m_shader->Bind();
	}

	static std::array<Vertex, 4> CreateQuad(float x, float y, float texId)
	{
		float size = 100.0f;
		// legend:    x         y         r      g     b      a     x     y     <id>
		Vertex v0 = { x,        y,        0.18f, 0.6f, 0.96f, 1.0f, 0.0f, 0.0f, texId };
		Vertex v1 = { x + size, y,        0.18f, 0.6f, 0.96f, 1.0f, 1.0f, 0.0f, texId };
		Vertex v2 = { x + size, y + size, 0.18f, 0.6f, 0.96f, 1.0f, 1.0f, 1.0f, texId };
		Vertex v3 = { x,        y + size, 0.18f, 0.6f, 0.96f, 1.0f, 0.0f, 1.0f, texId };

		return { v0, v1, v2, v3 };
	}

	void OnUpdate([[ maybe_unused ]] float deltaTime = 0.0f) override
	{
		auto q0 = CreateQuad(m_quad0Position[0], m_quad0Position[1], 0.f);
		auto q1 = CreateQuad(m_quad1Position[0], m_quad1Position[1], 1.f);

		std::vector<Vertex> batched_positions(q0.size() + q1.size());
		memcpy(batched_positions.data() /*       */, q0.data(), sizeof(Vertex) * q0.size());
		memcpy(batched_positions.data() + q0.size(), q1.data(), sizeof(Vertex) * q1.size());

		std::vector<unsigned int> indices1 = { // trig[v1,v2,v3...]
			0, 1, 2,
			2, 3, 0
		};
		std::vector<unsigned int> indices2 = { // must be shifted to not to corelate with `indices1`
			0, 1, 2, // 4, 5, 6
			2, 3, 0  // 6, 7, 4
		};

		// since we append 2nd positions buff - indices to be cancat should be shifted by the `amount` of positions in 1st pos buffer
		std::vector<unsigned int> batched_indices(indices1); // copy 1st buff
		std::transform(indices2.begin(), indices2.end(),     // append-shift 2nd buff
					   std::back_inserter(batched_indices),
					   [&](const auto &val) { return unsigned(val + q0.size()); });

		m_indexBuffer = std::make_unique<IndexBuffer>(batched_indices.data(), unsigned(batched_indices.size()));

		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_arrayBuffer));
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, batched_positions.size() * sizeof(Vertex), batched_positions.data()));
	}
	void OnRender() override
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_translation);
			glm::mat4 mvp = m_proj * m_view * model;

			m_shader->Bind();
			m_shader->SetUniformMat4f("u_MVP", mvp);

			m_chernoTex.Bind(0); m_hazelTex.Bind(1);
			m_shader->SetUniformVec1i("u_Textures", std::vector{ 0, 1 });

			GLCall(glBindVertexArray(m_vertexArray));
			m_indexBuffer->Bind();
			m_shader->Bind();

			GLCall(glDrawElements(GL_TRIANGLES, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr));
		}

	}
	void OnImGuiRender() override
	{
		ImGui::SliderFloat2("Translation", &m_translation.x, 0.0f, 960.0f);
		ImGui::SliderFloat2("Quad 1 'C'", m_quad0Position, 0.0f, 960.0f);
		ImGui::SliderFloat2("Quad 2 'H'", m_quad1Position, 0.0f, 960.0f);
	}
};

}
