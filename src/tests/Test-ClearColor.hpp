#pragma once

#include "Test.hpp"
#include "Utility.hpp"

#include <GL/glew.h>
#include <imgui/imgui.h>

namespace test
{

class TestClearColor : public Test
{
	float m_ClearColor[4] = { 0.2f, 0.3f, 0.8f, 1.0f };

public:
	TestClearColor() {}
	~TestClearColor() {}

	void OnUpdate(float deltaTime = 0.0f) override {}
	void OnRender() override
	{
		GLCall(glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
	}
	void OnImGuiRender() override { ImGui::ColorEdit3("Clear Color", m_ClearColor); }
};

}
