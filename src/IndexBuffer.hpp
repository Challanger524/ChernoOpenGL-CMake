#pragma once

#include "Utility.hpp"

#include <GL/glew.h>

class IndexBuffer
{
	unsigned int m_rendererId;
	unsigned int m_count;

public:
	IndexBuffer(const unsigned int *data, unsigned int count) : m_count(count)
	{
		static_assert(sizeof(m_count) == sizeof(GLuint));

		GLCall(glGenBuffers(1, &m_rendererId));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_rendererId));
		GLCall(glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), data, GL_STATIC_DRAW));
	}

	~IndexBuffer() { GLCall(glDeleteBuffers(1, &m_rendererId)); }

	void Bind() const { GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererId)); }
	void Unbind() const { GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)); }

	unsigned int GetCount() const { return m_count; }
};
