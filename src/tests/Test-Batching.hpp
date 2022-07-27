#pragma once

#include "Test.hpp"
#include "Utility.hpp"

#include "Renderer.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"

#include <GL/glew.h>
#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <vector>
#include <type_traits>

namespace test
{

class Batching : public Test
{
	std::unique_ptr<VertexArray> m_vao;
	std::unique_ptr<IndexBuffer> m_indexBuffer;
	std::unique_ptr<VertexBuffer> m_vertexBuffer;
	std::unique_ptr<Shader> m_shader;

	glm::mat4 m_proj = glm::ortho(0.0f, 960.0f, 0.0f, 720.0f, -1.0f, 1.0f);	   // screen scale
	glm::mat4 m_view = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0)); // camera
	glm::vec3 m_translation = glm::vec3(400, 200, 0);

	Renderer m_renderer;

public:
	~Batching() {}
	Batching()
	{
		// object 1
		std::vector<float> positions1 = { // pos[x,y], color[r,g,b,a], ...
			100.0f, 100.0f, 0.18f, 0.6f, 0.96f, 1.0f, // 0
			200.0f, 100.0f, 0.18f, 0.6f, 0.96f, 1.0f, // 1
			200.0f, 200.0f, 0.18f, 0.6f, 0.96f, 1.0f, // 2
			100.0f, 200.0f, 0.18f, 0.6f, 0.96f, 1.0f  // 3
		};
		std::vector<unsigned int> indices1 = { // trig[v1,v2,v3...]
			0, 1, 2,
			2, 3, 0
		};

		// object 2
		std::vector<float> positions2 = {
			300.0f, 100.0f, 1.0f, 0.93f, 0.24f, 1.0f, // 4
			400.0f, 100.0f, 1.0f, 0.93f, 0.24f, 1.0f, // 5
			400.0f, 200.0f, 1.0f, 0.93f, 0.24f, 1.0f, // 6
			300.0f, 200.0f, 1.0f, 0.93f, 0.24f, 1.0f  // 7
		};
		std::vector<unsigned int> indices2 = { // must be shifted to not to corelate with `indices1`
			0, 1, 2, // 4, 5, 6
			2, 3, 0  // 6, 7, 4
		};

		constexpr auto coord_count = 2u; // xy
		constexpr auto color_count = 4u; // rgba

		// batcheding the objects
		std::vector<float> batched_positions;
		batched_positions.insert(batched_positions.end(), positions1.begin(), positions1.end()); // append `pos1` buff
		batched_positions.insert(batched_positions.end(), positions2.begin(), positions2.end()); // append `pos2` buff

		// since we append 2nd positions buff - indices to be cancat should be shifted by the `amount` of positions in 1st pos buffer
		std::vector<unsigned int> batched_indices(indices1); // copy 1st buff
		std::transform(indices2.begin(), indices2.end(), // append-shift 2nd buff
					   std::back_inserter(batched_indices),
					   [&](const auto &val) { return unsigned int(val + positions1.size() / (coord_count + color_count)); });


		m_vao = std::make_unique<VertexArray>();

		static_assert(sizeof(float) == sizeof(decltype(batched_positions)::value_type));
		m_vertexBuffer = std::make_unique<VertexBuffer>(batched_positions.data(), unsigned(batched_positions.size() * sizeof(decltype(batched_positions)::value_type)));

		VertexBufferLayout layout;
		layout.Push<float>(2); // coord xy
		layout.Push<float>(4); // color rgb
		m_vao->AddBuffer(*m_vertexBuffer, layout);

		m_indexBuffer = std::make_unique<IndexBuffer>(batched_indices.data(), unsigned int(batched_indices.size()));

		m_shader = std::make_unique<Shader>("res/shaders/Batch.shader");
		m_shader->Bind();
	}

	void OnUpdate(float deltaTime = 0.0f) override {}
	void OnRender() override
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		{

			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_translation);
			glm::mat4 mvp = m_proj * m_view * model;


			m_shader->Bind();
			m_shader->SetUniformMat4f("u_MVP", mvp);

			m_renderer.Draw(*m_vao, *m_indexBuffer, *m_shader);
		}

	}
	void OnImGuiRender() override
	{
		ImGui::SliderFloat2("Translation", &m_translation.x, 0.0f, 960.0f);
	}
};

}
