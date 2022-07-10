#pragma once

#include "Utility.hpp"
#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"

#include <GL/glew.h>

class Renderer
{
public:
	void Clear() { GLCall(glClear(GL_COLOR_BUFFER_BIT)); }
	void Draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader) const
	{
		va.Bind();
		ib.Bind();
		shader.Bind();

		GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
	}
};
