#pragma once

#include "Utility.hpp"

#include <GL/glew.h>

class VertexBuffer
{
	unsigned int m_rendererId;

public:
	VertexBuffer(const void *data, unsigned int size)
	{
		GLCall(glGenBuffers(1, &m_rendererId));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_rendererId));
		GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
	}
	~VertexBuffer() { GLCall(glDeleteBuffers(1, &m_rendererId)); }

	void Bind() const { GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_rendererId)); }
	void Unbind() const { GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0)); }
};
