#pragma once

namespace test
{

class Test
{
public:
	Test() {}
	virtual ~Test() {}

	virtual void OnUpdate(float deltaTime = 0.0f) {}
	virtual void OnRender() {}
	virtual void OnImGuiRender() {}
};

}
