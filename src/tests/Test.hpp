#pragma once

#include <imgui/imgui.h>

#include <iostream>
#include <string>
#include <vector>
#include <functional>

namespace test
{

class Test
{
public:
	Test() {}
	virtual ~Test() {}

	virtual void OnUpdate([[maybe_unused]] float deltaTime = 0.0f) {}
	virtual void OnRender() {}
	virtual void OnImGuiRender() {}
};

class TestMenu : public Test
{
	using tests_t = std::vector<std::pair<std::string, std::function<Test *()>>>;

	Test *&m_currentTest;
	tests_t m_tests;

public:
	TestMenu(Test *&currentTestPtr) : m_currentTest(currentTestPtr) {}
	virtual ~TestMenu() override {}

	virtual void OnUpdate([[maybe_unused]] float deltaTime = 0.0f) override {}
	virtual void OnRender() override {}
	virtual void OnImGuiRender() override
	{
		for (auto &test : m_tests)
		{
			if (ImGui::Button(test.first.c_str()))
				m_currentTest = test.second();
		}
	}

	template<class T>
	void RegisterTest(const std::string &name)
	{
		std::cout << "Trace: Registering test: " << name << '\n';
		m_tests.push_back(std::make_pair(name, []() { return new T(); }));
	}
};

}
